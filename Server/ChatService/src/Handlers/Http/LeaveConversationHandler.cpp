#include "Handlers/Http/LeaveConversationHandler.hpp"

#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/options.hpp>
#include <userver/utils/datetime.hpp>
#include <userver/utils/from_string.hpp>

namespace chat_service::http_handlers {

namespace {
const userver::storages::postgres::Query kCheckActiveParticipantForLeave{
    "SELECT c.type, p.role "
    "FROM conversations c "
    "JOIN participants p ON c.id = p.conversation_id "
    "WHERE c.id = $1 AND p.user_id = $2 AND c.deleted = FALSE AND p.left_at IS "
    "NULL",
    userver::storages::postgres::Query::Name{
        "check_active_participant_for_leave_v2"},
};

const userver::storages::postgres::Query kSetCurrentUserLeftAt{
    "UPDATE participants SET left_at = NOW() "
    "WHERE conversation_id = $1 AND user_id = $2 AND left_at IS NULL",
    userver::storages::postgres::Query::Name{"set_current_user_left_at_v3"},
};

const userver::storages::postgres::Query kCountActiveAdminsForLeave{
    "SELECT COUNT(*) FROM participants "
    "WHERE conversation_id = $1 AND role = 'admin' AND left_at IS NULL",
    userver::storages::postgres::Query::Name{
        "count_active_admins_for_leave_v2"},
};

const std::string kConversationTypeGroupStr = "group";
const std::string kParticipantRoleAdminStr = "admin";

} // namespace

LeaveConversationHandler::LeaveConversationHandler(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &component_context)
    : HttpHandlerJsonBase(config, component_context),
      pg_cluster_(component_context
                      .FindComponent<userver::components::Postgres>("chat-db")
                      .GetCluster()) {}

LeaveConversationHandler::ConversationLeaveCheckDetails
LeaveConversationHandler::CheckUserParticipation(
    int conversation_id, int64_t current_user_id) const {

  ConversationLeaveCheckDetails details{false, "", ""};

  auto check_result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      kCheckActiveParticipantForLeave, conversation_id, current_user_id);

  if (check_result.IsEmpty()) {
    details.conversation_exists_and_active_participant = false;
    return details;
  }

  details.conversation_exists_and_active_participant = true;
  const auto &row = check_result.Front();
  details.conversation_type_str = row["type"].As<std::string>();
  details.current_user_role_str = row["role"].As<std::string>();

  return details;
}

LeaveConversationHandler::ResponseType
LeaveConversationHandler::HandleRequestJsonThrow(
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

  ConversationLeaveCheckDetails check_details =
      CheckUserParticipation(conversation_id_val, current_user_id_val);

  if (!check_details.conversation_exists_and_active_participant) {
    http_request.SetResponseStatus(
        userver::server::http::HttpStatus::kNotFound);
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"NOT_FOUND_OR_NOT_PARTICIPANT","message":"Conversation not found or you are not an active participant."})"});
  }

  if (check_details.conversation_type_str == kConversationTypeGroupStr &&
      check_details.current_user_role_str == kParticipantRoleAdminStr) {

    auto admin_count_res = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        kCountActiveAdminsForLeave, conversation_id_val);
    if (!admin_count_res.IsEmpty() &&
        admin_count_res.Front()[0].As<int64_t>() <= 1) {
      http_request.SetResponseStatus(
          userver::server::http::HttpStatus::kBadRequest);
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              R"({"code":"LAST_ADMIN_CANNOT_LEAVE","message":"The last admin cannot leave the group."})"});
    }
  }

  auto trx = pg_cluster_->Begin(
      "trx", userver::storages::postgres::ClusterHostType::kMaster, {});
  try {
    auto update_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        kSetCurrentUserLeftAt, conversation_id_val, current_user_id_val);

    if (update_result.RowsAffected() > 0) {
      LOG_INFO() << "User " << current_user_id_val << " left conversation "
                 << conversation_id_val;
    } else {
      LOG_INFO() << "User " << current_user_id_val
                 << " was already marked as left from conversation "
                 << conversation_id_val << " when attempting to leave.";
    }

    trx.Commit();

  } catch (const std::exception &e) {
    LOG_ERROR() << "Generic error while leaving conversation: " << e.what();
    trx.Rollback();
    throw userver::server::handlers::InternalServerError(
        userver::server::handlers::ExternalBody{
            R"({"code":"INTERNAL_ERROR","message":"An unexpected error occurred."})"});
  }

  http_request.SetResponseStatus(userver::server::http::HttpStatus::kNoContent);
  return {};
}

} // namespace chat_service::http_handlers