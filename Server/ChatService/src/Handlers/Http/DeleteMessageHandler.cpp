#include "Handlers/Http/DeleteMessageHandler.hpp"

#include <userver/formats/serialize/common_containers.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/options.hpp>
#include <userver/utils/datetime.hpp>
#include <userver/utils/from_string.hpp>

namespace chat_service::http_handlers {

namespace {
const userver::storages::postgres::Query kGetMessageDetailsForDelete{
    "SELECT conversation_id, sender_id, deleted FROM messages WHERE id = $1",
    userver::storages::postgres::Query::Name{
        "get_message_details_for_delete_v2"},
};

const userver::storages::postgres::Query kSoftDeleteMessage{
    "UPDATE messages SET deleted = TRUE, deletion_reason = 'user', edited_at = "
    "NOW() "
    "WHERE id = $1 AND deleted = FALSE",
    userver::storages::postgres::Query::Name{"soft_delete_message_v4"},
};

const userver::storages::postgres::Query kUpdateConversationOnDelete{
    "UPDATE conversations SET updated_at = NOW() WHERE id = $1",
    userver::storages::postgres::Query::Name{
        "update_conversation_on_delete_v4"},
};
} // namespace

DeleteMessageHandler::DeleteMessageHandler(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &component_context)
    : HttpHandlerJsonBase(config, component_context),
      pg_cluster_(component_context
                      .FindComponent<userver::components::Postgres>("chat-db")
                      .GetCluster()) {}

DeleteMessageHandler::MessageDetailsForDelete
DeleteMessageHandler::GetMessageDetailsAndCheckPermissions(
    int64_t message_id, int64_t current_user_id) const {
  auto result =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           kGetMessageDetailsForDelete, message_id);

  if (result.IsEmpty()) {
    return {0, 0, false, false};
  }

  const auto row = result.Front();
  int conversation_id_val = row["conversation_id"].As<int>();
  int64_t sender_id_val = row["sender_id"].As<int64_t>();
  bool already_deleted = row["deleted"].As<bool>();

  if (sender_id_val != current_user_id) {
    return {0, 0, false, already_deleted};
  }
  return {conversation_id_val, sender_id_val, true, already_deleted};
}

DeleteMessageHandler::ResponseType DeleteMessageHandler::HandleRequestJsonThrow(
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

  const auto &message_id_str = http_request.GetPathArg("message_id");
  int64_t message_id_val;
  try {
    message_id_val = userver::utils::FromString<int64_t>(message_id_str);
  } catch (const std::exception &e) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"INVALID_ARGUMENT","message":"Invalid message_id format"})"});
  }

  MessageDetailsForDelete message_details =
      GetMessageDetailsAndCheckPermissions(message_id_val, current_user_id_val);

  if (!message_details.exists) {
    http_request.SetResponseStatus(
        userver::server::http::HttpStatus::kNotFound);
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"NOT_FOUND","message":"Message not found or permission denied."})"});
  }

  if (message_details.already_deleted) {
    LOG_INFO() << "Attempt to delete already deleted message " << message_id_val
               << " by user " << current_user_id_val;
    http_request.SetResponseStatus(
        userver::server::http::HttpStatus::kNoContent);
    return {};
  }

  auto trx = pg_cluster_->Begin(
      "trx", userver::storages::postgres::ClusterHostType::kMaster, {});
  try {
    auto delete_result = trx.Execute(kSoftDeleteMessage, message_id_val);

    if (delete_result.RowsAffected() == 0) {
      LOG_WARNING() << "Message " << message_id_val
                    << " was not soft-deleted, possibly already deleted by "
                       "another request.";
    }

    if (message_details.conversation_id_val > 0) {
      trx.Execute(kUpdateConversationOnDelete,
                  message_details.conversation_id_val);
    }

    trx.Commit();
    LOG_INFO() << "User " << current_user_id_val << " soft-deleted message "
               << message_id_val;

  } catch (const std::exception &e) {
    LOG_ERROR() << "Generic error during message soft-deletion: " << e.what();
    trx.Rollback();
    throw userver::server::handlers::InternalServerError(
        userver::server::handlers::ExternalBody{
            R"({"code":"INTERNAL_ERROR","message":"An unexpected error occurred"})"});
  }

  http_request.SetResponseStatus(userver::server::http::HttpStatus::kNoContent);
  return {};
}

} // namespace chat_service::http_handlers