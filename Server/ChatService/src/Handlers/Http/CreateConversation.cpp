#include "Handlers/Http/CreateConversation.hpp"

#include <userver/components/component.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/date.hpp>
#include <userver/storages/postgres/options.hpp>
#include <userver/utils/datetime.hpp>
#include <vector>

#include <userver/formats/serialize/common_containers.hpp>
namespace chat_service::http_handlers {

namespace {

const userver::storages::postgres::Query kInsertConversation{
    "INSERT INTO conversations (type, title) "
    "VALUES ($1, $2) "
    "RETURNING id, created_at, updated_at, type, title",
    userver::storages::postgres::Query::Name{"insert_conversation_v3"},
};

const userver::storages::postgres::Query kInsertParticipant{
    "INSERT INTO participants (conversation_id, user_id, role) "
    "VALUES ($1, $2, $3)",
    userver::storages::postgres::Query::Name{"insert_participant_v3"},
};

const std::string kConversationTypeGroup = "group";
const std::string kConversationTypePrivate = "private";
const std::string kParticipantRoleAdmin = "admin";
const std::string kParticipantRoleMember = "member";

} // namespace

CreateConversationHandler::CreateConversationHandler(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &component_context)
    : HttpHandlerJsonBase(config, component_context),
      pg_cluster_(component_context
                      .FindComponent<userver::components::Postgres>("chat-db")
                      .GetCluster()) {}

CreateConversationHandler::ResponseType
CreateConversationHandler::HandleRequestJsonThrow(
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
    error_builder["message"] =
        "User not authenticated or user_id missing in context";
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            "User not authenticated or user_id missing in context"});
  }

  if (!request_json.HasMember("type") || !request_json["type"].IsString()) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"VALIDATION_ERROR","message":"'type' field is required and must be a string."})"});
  }
  std::string type_str = request_json["type"].As<std::string>();

  if (!request_json.HasMember("participant_user_ids") ||
      !request_json["participant_user_ids"].IsArray()) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"VALIDATION_ERROR","message":"'participant_user_ids' field is required and must be an array."})"});
  }
  std::vector<int64_t> participant_user_ids =
      request_json["participant_user_ids"].As<std::vector<int64_t>>();

  std::optional<std::string> title_opt;
  if (request_json.HasMember("title") && request_json["title"].IsString()) {
    title_opt = request_json["title"].As<std::string>();
  } else if (request_json.HasMember("title") &&
             !request_json["title"].IsNull()) {

    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"VALIDATION_ERROR","message":"'title' field must be a string if provided."})"});
  }

  if (type_str == kConversationTypeGroup) {
    if (!title_opt || title_opt->empty()) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              R"({"code":"VALIDATION_ERROR","message":"Title is required for group chats"})"});
    }
    if (participant_user_ids.empty()) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              R"({"code":"VALIDATION_ERROR","message":"Group chat must have at least one other participant"})"});
    }
  } else if (type_str == kConversationTypePrivate) {
    if (participant_user_ids.size() != 1) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              R"({"code":"VALIDATION_ERROR","message":"Private chat must have exactly one other participant"})"});
    }
    if (participant_user_ids[0] == current_user_id_val) {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              R"({"code":"VALIDATION_ERROR","message":"Cannot create a private chat with yourself"})"});
    }
  } else {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"VALIDATION_ERROR","message":"Invalid conversation type. Allowed: 'private', 'group'."})"});
  }

  auto trx = pg_cluster_->Begin(
      "trx", userver::storages::postgres::ClusterHostType::kMaster, {});
  int new_conv_id_val;
  userver::formats::json::ValueBuilder response_builder;

  try {

    auto conv_result = trx.Execute(kInsertConversation, type_str, title_opt);
    if (conv_result.IsEmpty()) {
      throw std::runtime_error(
          "DB error: Conversation creation failed unexpectedly.");
    }
    const auto &row = conv_result.Front();
    new_conv_id_val = row["id"].As<int>();

    response_builder["id"] = new_conv_id_val;
    response_builder["type"] = row["type"].As<std::string>();
    if (!row["title"].IsNull()) {
      response_builder["title"] = row["title"].As<std::string>();
    } else if (type_str == kConversationTypeGroup) {
      response_builder["title"] = title_opt.value_or("");
    }
    response_builder["created_at"] =
        row["created_at"].As<std::chrono::system_clock::time_point>();
    response_builder["updated_at"] =
        row["updated_at"].As<std::chrono::system_clock::time_point>();

    std::string creator_role_str = (type_str == kConversationTypeGroup)
                                       ? kParticipantRoleAdmin
                                       : kParticipantRoleMember;
    trx.Execute(kInsertParticipant, new_conv_id_val, current_user_id_val,
                creator_role_str);

    for (const auto &participant_id_val : participant_user_ids) {
      trx.Execute(kInsertParticipant, new_conv_id_val, participant_id_val,
                  kParticipantRoleMember);
    }

    trx.Commit();
    LOG_INFO() << "Successfully created conversation with ID: "
               << new_conv_id_val;

  } catch (const std::exception &e) {
    LOG_ERROR() << "Generic error during conversation creation: " << e.what();
    trx.Rollback();
    throw userver::server::handlers::InternalServerError(
        userver::server::handlers::ExternalBody{
            R"({"code":"INTERNAL_ERROR","message":"An unexpected error occurred"})"});
  }

  http_request.SetResponseStatus(userver::server::http::HttpStatus::kCreated);
  return response_builder.ExtractValue();
}

} // namespace chat_service::http_handlers