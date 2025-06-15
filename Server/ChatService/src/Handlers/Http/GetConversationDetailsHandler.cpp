#include "Handlers/Http/GetConversationDetailsHandler.hpp"

#include <optional>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/options.hpp>
#include <userver/utils/datetime.hpp>
#include <userver/utils/datetime/timepoint_tz.hpp>
#include <userver/utils/from_string.hpp>

namespace chat_service::http_handlers {

namespace {
const userver::storages::postgres::Query kGetConversationBaseInfoDetails{
    "SELECT id, type, title, last_message_id, created_at, updated_at "
    "FROM conversations "
    "WHERE id = $1 AND deleted = FALSE",
    userver::storages::postgres::Query::Name{
        "get_conversation_base_info_details_v3"},
};

const userver::storages::postgres::Query kGetActiveParticipantsDetails{
    "SELECT user_id, role, joined_at, last_read_message_id, "
    "notifications_enabled "
    "FROM participants "
    "WHERE conversation_id = $1 AND left_at IS NULL "
    "ORDER BY joined_at ASC",
    userver::storages::postgres::Query::Name{
        "get_active_participants_details_v3"},
};

const userver::storages::postgres::Query kCheckIsActiveParticipantForDetails{
    "SELECT 1 FROM participants WHERE conversation_id = $1 AND user_id = $2 "
    "AND left_at IS NULL LIMIT 1",
    userver::storages::postgres::Query::Name{
        "check_is_active_participant_details_v3"},
};
} // namespace

GetConversationDetailsHandler::GetConversationDetailsHandler(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &component_context)
    : HttpHandlerJsonBase(config, component_context),
      pg_cluster_(component_context
                      .FindComponent<userver::components::Postgres>("chat-db")
                      .GetCluster()) {}

bool GetConversationDetailsHandler::IsUserActiveParticipant(
    int conversation_id, int64_t current_user_id) const {
  auto result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      kCheckIsActiveParticipantForDetails, conversation_id, current_user_id);
  return !result.IsEmpty();
}

GetConversationDetailsHandler::ResponseType
GetConversationDetailsHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest &http_request, const RequestType &,
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

  if (!IsUserActiveParticipant(conversation_id_val, current_user_id_val)) {
    http_request.SetResponseStatus(
        userver::server::http::HttpStatus::kForbidden);
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"FORBIDDEN_OR_NOT_FOUND","message":"Conversation not found or you are not a participant."})"});
  }

  userver::formats::json::ValueBuilder response_builder(
      userver::formats::json::Type::kObject);

  try {

    auto conv_base_info_res = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        kGetConversationBaseInfoDetails, conversation_id_val);

    if (conv_base_info_res.IsEmpty()) {
      http_request.SetResponseStatus(
          userver::server::http::HttpStatus::kNotFound);
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              R"({"code":"NOT_FOUND","message":"Conversation not found."})"});
    }

    const auto &conv_row = conv_base_info_res.Front();
    response_builder["id"] = conv_row["id"].As<std::optional<int>>();
    response_builder["type"] =
        conv_row["type"].As<std::optional<std::string>>();

    if (!conv_row["title"].IsNull()) {
      response_builder["title"] =
          conv_row["title"].As<std::optional<std::string>>();
    }
    if (!conv_row["last_message_id"].IsNull()) {
      response_builder["last_message_id"] =
          conv_row["last_message_id"].As<std::optional<int64_t>>();
    }
    response_builder["created_at"] =
        std::optional<userver::utils::datetime::TimePointTz>(
            conv_row["created_at"]
                .As<userver::storages::postgres::TimePointTz>()
                .GetUnderlying());
    response_builder["updated_at"] =
        std::optional<userver::utils::datetime::TimePointTz>(
            conv_row["updated_at"]
                .As<userver::storages::postgres::TimePointTz>()
                .GetUnderlying());

    auto participants_res = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        kGetActiveParticipantsDetails, conversation_id_val);

    userver::formats::json::ValueBuilder participants_array_builder(
        userver::formats::json::Type::kArray);
    for (const auto &participant_row : participants_res) {
      userver::formats::json::ValueBuilder p_builder(
          userver::formats::json::Type::kObject);
      p_builder["user_id"] =
          participant_row["user_id"].As<std::optional<int64_t>>();
      p_builder["role"] =
          participant_row["role"].As<std::optional<std::string>>();
      p_builder["joined_at"] =
          std::optional<userver::utils::datetime::TimePointTz>(
              participant_row["joined_at"]
                  .As<userver::storages::postgres::TimePointTz>()
                  .GetUnderlying());

      if (!participant_row["last_read_message_id"].IsNull()) {
        p_builder["last_read_message_id"] =
            participant_row["last_read_message_id"]
                .As<std::optional<int64_t>>();
      }
      if (!participant_row["notifications_enabled"].IsNull()) {
        p_builder["notifications_enabled"] =
            participant_row["notifications_enabled"].As<std::optional<bool>>();
      }
      participants_array_builder.PushBack(p_builder.ExtractValue());
    }
    response_builder["participants"] =
        participants_array_builder.ExtractValue();

  } catch (const std::exception &e) {
    LOG_ERROR() << "Generic error while fetching conversation details: "
                << e.what();
    throw userver::server::handlers::InternalServerError(
        userver::server::handlers::ExternalBody{
            R"({"code":"INTERNAL_ERROR","message":"An unexpected error occurred"})"});
  }

  http_request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
  return response_builder.ExtractValue();
}

} // namespace chat_service::http_handlers