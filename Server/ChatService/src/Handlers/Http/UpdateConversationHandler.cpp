#include "Handlers/Http/UpdateConversationHandler.hpp"

#include <optional>
#include <userver/formats/json/value_builder.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/options.hpp>
#include <userver/utils/datetime.hpp>
#include <userver/utils/from_string.hpp>

#include <userver/formats/serialize/common_containers.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/storages/postgres/transaction.hpp>
#include <userver/utils/datetime/timepoint_tz.hpp>
namespace chat_service::http_handlers {

namespace {
const userver::storages::postgres::Query kCheckConversationAdminForUpdate{
    "SELECT c.type, p.role "
    "FROM conversations c "
    "JOIN participants p ON c.id = p.conversation_id "
    "WHERE c.id = $1 AND p.user_id = $2 AND c.deleted = FALSE AND p.left_at IS "
    "NULL",
    userver::storages::postgres::Query::Name{
        "check_conversation_admin_for_update_v3"},
};

const userver::storages::postgres::Query kUpdateConversationDetails{
    "UPDATE conversations SET title = $1, updated_at = NOW() "
    "WHERE id = $2 "
    "RETURNING id, type, title, last_message_id, created_at, updated_at",
    userver::storages::postgres::Query::Name{"update_conversation_details_v4"},
};

const std::string kConversationTypeGroupStr = "group";
const std::string kParticipantRoleAdminStr = "admin";

} // namespace

UpdateConversationHandler::UpdateConversationHandler(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &component_context)
    : HttpHandlerJsonBase(config, component_context),
      pg_cluster_(component_context
                      .FindComponent<userver::components::Postgres>("chat-db")
                      .GetCluster()) {}

UpdateConversationHandler::ConversationUpdateCheckDetails
UpdateConversationHandler::CheckPermissionsForUpdate(
    int conversation_id, int64_t current_user_id) const {

  ConversationUpdateCheckDetails details{false, "", false};

  auto check_result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      kCheckConversationAdminForUpdate, conversation_id, current_user_id);

  if (check_result.IsEmpty()) {
    details.conversation_exists_and_active_participant = false;
    return details;
  }

  details.conversation_exists_and_active_participant = true;
  const auto &row = check_result.Front();
  details.conversation_type_str = row["type"].As<std::string>();
  if (row["role"].As<std::string>() == kParticipantRoleAdminStr) {
    details.current_user_is_admin = true;
  }

  return details;
}

UpdateConversationHandler::ResponseType
UpdateConversationHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest &http_request,
    const RequestType &request_json,
    userver::server::request::RequestContext &request_context) const {

  int64_t current_user_id_val;
  try {
    current_user_id_val = request_context.GetData<int64_t>("user_id");
  } catch (const std::out_of_range &e) {
    http_request.SetResponseStatus(
        userver::server::http::HttpStatus::kUnauthorized);
    userver::formats::json::ValueBuilder error_builder;
    error_builder["code"] = "UNAUTHORIZED";
    error_builder["message"] = "User not authenticated";
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            "User not authenticated or user_id missing in context"});
  }

  const auto &conversation_id_str = http_request.GetPathArg("conversation_id");
  int conversation_id_val;
  try {
    conversation_id_val = userver::utils::FromString<int>(conversation_id_str);
  } catch (const std::exception &e) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"INVALID_ARGUMENT","message":"Invalid conversation_id format"})"});
  }

  if (!request_json.HasMember("title") || !request_json["title"].IsString()) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"VALIDATION_ERROR","message":"'title' field is required and must be a string."})"});
  }
  std::string title_str = request_json["title"].As<std::string>();

  if (title_str.empty() || title_str.length() > 100) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"VALIDATION_ERROR","message":"Title must be between 1 and 100 characters."})"});
  }

  ConversationUpdateCheckDetails check_details =
      CheckPermissionsForUpdate(conversation_id_val, current_user_id_val);

  if (!check_details.conversation_exists_and_active_participant) {
    http_request.SetResponseStatus(
        userver::server::http::HttpStatus::kNotFound);
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"NOT_FOUND_OR_NOT_PARTICIPANT","message":"Conversation not found or you are not an active participant."})"});
  }
  if (check_details.conversation_type_str != kConversationTypeGroupStr) {
    http_request.SetResponseStatus(
        userver::server::http::HttpStatus::kBadRequest);
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"INVALID_OPERATION","message":"Only group conversation information can be updated."})"});
  }
  if (!check_details.current_user_is_admin) {
    http_request.SetResponseStatus(
        userver::server::http::HttpStatus::kForbidden);
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"FORBIDDEN","message":"Only administrators can update conversation information."})"});
  }

  userver::formats::json::ValueBuilder response_builder(
      userver::formats::json::Type::kObject);
  auto trx = pg_cluster_->Begin(
      "trx", userver::storages::postgres::ClusterHostType::kMaster, {});

  try {
    auto update_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        kUpdateConversationDetails, title_str, conversation_id_val);

    if (update_result.IsEmpty()) {
      throw std::runtime_error(
          "DB error: Conversation update failed unexpectedly.");
    }

    const auto &row = update_result.Front();
    response_builder["id"] = row["id"].As<int>();
    response_builder["type"] = row["type"].As<std::string>();
    if (!row["title"].IsNull()) {
      response_builder["title"] = row["title"].As<std::string>();
    }
    if (!row["last_message_id"].IsNull()) {
      response_builder["last_message_id"] =
          row["last_message_id"].As<int64_t>();
    }
    response_builder["created_at"] =
        std::optional<userver::utils::datetime::TimePointTz>(
            row["created_at"]
                .As<userver::storages::postgres::TimePointTz>()
                .GetUnderlying());
    response_builder["updated_at"] =
        std::optional<userver::utils::datetime::TimePointTz>(
            row["updated_at"]
                .As<userver::storages::postgres::TimePointTz>()
                .GetUnderlying());

    trx.Commit();
    LOG_INFO() << "Admin " << current_user_id_val
               << " updated title of conversation " << conversation_id_val
               << " to '" << title_str << "'";

  } catch (const std::exception &e) {
    LOG_ERROR() << "Generic error during conversation update: " << e.what();
    trx.Rollback();
    throw userver::server::handlers::InternalServerError(
        userver::server::handlers::ExternalBody{
            R"({"code":"INTERNAL_ERROR","message":"An unexpected error occurred."})"});
  }

  http_request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return response_builder.ExtractValue();
}

} // namespace chat_service::http_handlers