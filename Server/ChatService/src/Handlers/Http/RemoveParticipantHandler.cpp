#include "Handlers/Http/RemoveParticipantHandler.hpp"

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
const userver::storages::postgres::Query kCheckDetailsForParticipantRemoval{
    "SELECT "
    "   c.type AS conversation_type, "
    "   (SELECT p_admin.role FROM participants p_admin WHERE "
    "p_admin.conversation_id = c.id AND p_admin.user_id = $2 AND "
    "p_admin.left_at IS NULL) AS current_user_role, "
    "   (SELECT p_target.role FROM participants p_target WHERE "
    "p_target.conversation_id = c.id AND p_target.user_id = $3 AND "
    "p_target.left_at IS NULL) AS target_user_role "
    "FROM conversations c "
    "WHERE c.id = $1 AND c.deleted = FALSE",
    userver::storages::postgres::Query::Name{
        "check_details_for_participant_removal_v2"},
};

const userver::storages::postgres::Query kSetParticipantLeftAt{
    "UPDATE participants SET left_at = NOW() "
    "WHERE conversation_id = $1 AND user_id = $2 AND left_at IS NULL",
    userver::storages::postgres::Query::Name{"set_participant_left_at_v3"},
};

const userver::storages::postgres::Query kCountActiveAdminsForRemove{
    "SELECT COUNT(*) FROM participants "
    "WHERE conversation_id = $1 AND role = 'admin' AND left_at IS NULL",
    userver::storages::postgres::Query::Name{
        "count_active_admins_for_remove_v2"},
};

const std::string kConversationTypeGroupStr = "group";
const std::string kParticipantRoleAdminStr = "admin";

} // namespace

RemoveParticipantHandler::RemoveParticipantHandler(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &component_context)
    : HttpHandlerJsonBase(config, component_context),
      pg_cluster_(component_context
                      .FindComponent<userver::components::Postgres>("chat-db")
                      .GetCluster()) {}

RemoveParticipantHandler::ParticipantRemovalCheckDetails
RemoveParticipantHandler::CheckPermissionsForRemoval(
    int conversation_id, int64_t current_user_id,
    int64_t user_id_to_remove) const {

  ParticipantRemovalCheckDetails details{false, "", false, false, ""};

  auto check_result =
      pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave,
                           kCheckDetailsForParticipantRemoval, conversation_id,
                           current_user_id, user_id_to_remove);

  if (check_result.IsEmpty()) {
    details.conversation_exists = false;
    return details;
  }
  details.conversation_exists = true;

  const auto &row = check_result.Front();
  details.conversation_type_str = row["conversation_type"].As<std::string>();

  auto current_user_role_str_opt =
      row["current_user_role"].As<std::optional<std::string>>();
  if (current_user_role_str_opt.has_value() &&
      *current_user_role_str_opt == kParticipantRoleAdminStr) {
    details.current_user_is_admin = true;
  } else if (!current_user_role_str_opt.has_value()) {
    details.conversation_exists = false;
    return details;
  }

  auto target_user_role_str_opt =
      row["target_user_role"].As<std::optional<std::string>>();
  if (target_user_role_str_opt.has_value()) {
    details.target_user_is_participant = true;
    details.target_user_role_str = *target_user_role_str_opt;
  }

  return details;
}

RemoveParticipantHandler::ResponseType
RemoveParticipantHandler::HandleRequestJsonThrow(
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
  const auto &user_id_to_remove_str =
      http_request.GetPathArg("user_id_to_remove");

  int conversation_id_val;
  int64_t user_id_to_remove_val;

  try {
    conversation_id_val = userver::utils::FromString<int>(conversation_id_str);
    user_id_to_remove_val =
        userver::utils::FromString<int64_t>(user_id_to_remove_str);
  } catch (const std::exception &e) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"INVALID_ARGUMENT","message":"Invalid ID format in path."})"});
  }

  if (current_user_id_val == user_id_to_remove_val) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"INVALID_OPERATION","message":"Cannot remove yourself. Use /leave."})"});
  }

  ParticipantRemovalCheckDetails check_details = CheckPermissionsForRemoval(
      conversation_id_val, current_user_id_val, user_id_to_remove_val);

  if (!check_details.conversation_exists) {
    http_request.SetResponseStatus(
        userver::server::http::HttpStatus::kNotFound);
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"NOT_FOUND","message":"Conversation not found or no access."})"});
  }
  if (check_details.conversation_type_str != kConversationTypeGroupStr) {
    http_request.SetResponseStatus(
        userver::server::http::HttpStatus::kBadRequest);
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"INVALID_OPERATION","message":"Participants can only be removed from group."})"});
  }
  if (!check_details.current_user_is_admin) {
    http_request.SetResponseStatus(
        userver::server::http::HttpStatus::kForbidden);
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"FORBIDDEN","message":"Only admins can remove participants."})"});
  }
  if (!check_details.target_user_is_participant) {
    http_request.SetResponseStatus(
        userver::server::http::HttpStatus::kNotFound);
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            R"({"code":"TARGET_NOT_PARTICIPANT","message":"Target user is not an active participant."})"});
  }

  if (check_details.target_user_role_str == kParticipantRoleAdminStr) {
    auto admin_count_res = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        kCountActiveAdminsForRemove, conversation_id_val);
    if (!admin_count_res.IsEmpty() &&
        admin_count_res.Front()[0].As<int64_t>() <= 1) {
      http_request.SetResponseStatus(
          userver::server::http::HttpStatus::kBadRequest);
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{
              R"({"code":"LAST_ADMIN","message":"Cannot remove the last admin."})"});
    }
  }

  auto trx = pg_cluster_->Begin(
      "trx", userver::storages::postgres::ClusterHostType::kMaster, {});
  try {
    auto update_result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        kSetParticipantLeftAt, conversation_id_val, user_id_to_remove_val);

    if (update_result.RowsAffected() > 0) {
      LOG_INFO() << "Admin " << current_user_id_val << " removed participant "
                 << user_id_to_remove_val << " from conversation "
                 << conversation_id_val;
    } else {
      LOG_INFO()
          << "User " << user_id_to_remove_val
          << " was not an active participant or already left conversation "
          << conversation_id_val << " when admin " << current_user_id_val
          << " tried to remove them.";
    }

    trx.Commit();

  } catch (const std::exception &e) {
    LOG_ERROR() << "Generic error during participant removal: " << e.what();
    trx.Rollback();
    throw userver::server::handlers::InternalServerError(
        userver::server::handlers::ExternalBody{
            R"({"code":"INTERNAL_ERROR","message":"An unexpected error occurred."})"});
  }

  http_request.SetResponseStatus(userver::server::http::HttpStatus::kNoContent);
  return {};
}

} // namespace chat_service::http_handlers