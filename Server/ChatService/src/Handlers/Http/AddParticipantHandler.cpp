#include "Handlers/Http/AddParticipantHandler.hpp"

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

namespace chat_service::http_handlers
{

	namespace
	{
		const userver::storages::postgres::Query kCheckChatAdminForAdd{
			"SELECT c.type, p.role "
			"FROM conversations c "
			"JOIN participants p ON c.id = p.conversation_id "
			"WHERE c.id = $1 AND p.user_id = $2 AND c.deleted = FALSE AND "
			"p.left_at IS "
			"NULL",
			userver::storages::postgres::Query::Name{
				"check_chat_admin_for_add_v3"},
		};

		const userver::storages::postgres::Query
			kCheckTargetParticipantStatusForAdd{
				"SELECT role, left_at FROM participants WHERE conversation_id "
				"= $1 AND "
				"user_id = $2",
				userver::storages::postgres::Query::Name{
					"check_target_participant_status_for_add_v3"},
			};

		const userver::storages::postgres::Query kUpsertParticipant{
			"INSERT INTO participants (conversation_id, user_id, role, "
			"left_at, "
			"last_read_message_id, notifications_enabled) "

			"VALUES ($1, $2, $3, NULL, NULL, TRUE) "
			"ON CONFLICT (conversation_id, user_id) DO UPDATE SET "
			"role = EXCLUDED.role, "
			"joined_at = CASE WHEN participants.left_at IS NOT NULL THEN NOW() "
			"ELSE "
			"participants.joined_at END, "
			"left_at = NULL, "
			"last_read_message_id = NULL, "
			"notifications_enabled = TRUE "
			"RETURNING user_id, role, joined_at",
			userver::storages::postgres::Query::Name{"upsert_participant_v4"},
		};

		const std::string kConversationTypeGroupStr = "group";

		const std::string kParticipantRoleAdminStr = "admin";
		const std::string kParticipantRoleMemberStr = "member";

	} // namespace

	AddParticipantHandler::AddParticipantHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& component_context)
		: HttpHandlerJsonBase(config, component_context),
		  pg_cluster_(
			  component_context
				  .FindComponent<userver::components::Postgres>("chat-db")
				  .GetCluster())
	{
	}

	AddParticipantHandler::ConversationAccessDetails
	AddParticipantHandler::CheckConversationAndPermissions(
		int conversation_id, int64_t current_user_id,
		int64_t target_user_id) const
	{
		ConversationAccessDetails details{false, "", false, false, false};

		auto admin_check_result = pg_cluster_->Execute(
			userver::storages::postgres::ClusterHostType::kSlave,
			kCheckChatAdminForAdd, conversation_id, current_user_id);

		if (admin_check_result.IsEmpty())
		{
			return details;
		}
		details.exists = true;
		details.conversation_type_str =
			admin_check_result.Front()["type"].As<std::string>();
		details.current_user_is_admin =
			(admin_check_result.Front()["role"].As<std::string>() ==
			 kParticipantRoleAdminStr);

		auto target_status_result = pg_cluster_->Execute(
			userver::storages::postgres::ClusterHostType::kSlave,
			kCheckTargetParticipantStatusForAdd, conversation_id,
			target_user_id);

		if (!target_status_result.IsEmpty())
		{
			if (target_status_result.Front()["left_at"]
					.As<std::optional<std::chrono::system_clock::time_point>>()
					.has_value())
			{
				details.target_user_was_participant = true;
			}
			else
			{
				details.target_user_is_participant = true;
			}
		}
		return details;
	}

	AddParticipantHandler::ResponseType
	AddParticipantHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& http_request,
		const RequestType& request_json,
		userver::server::request::RequestContext& request_context) const
	{
		int64_t current_user_id_val;
		try
		{
			current_user_id_val = request_context.GetData<int64_t>("user_id");
		}
		catch (const std::out_of_range& e)
		{
			http_request.SetResponseStatus(
				userver::server::http::HttpStatus::kUnauthorized);
			userver::formats::json::ValueBuilder error_builder;
			error_builder["code"] = "UNAUTHORIZED";
			error_builder["message"] = "User not authenticated";
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					"User not authenticated"});
		}

		const auto& conversation_id_str =
			http_request.GetPathArg("conversation_id");
		int conversation_id_val;
		try
		{
			conversation_id_val =
				userver::utils::FromString<int>(conversation_id_str);
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"INVALID_ARGUMENT","message":"Invalid conversation_id format"})"});
		}

		if (!request_json.HasMember("user_id") ||
			!request_json["user_id"].IsInt64())
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"VALIDATION_ERROR","message":"'user_id' field is required and must be an integer."})"});
		}
		int64_t target_user_id_val = request_json["user_id"].As<int64_t>();

		std::string target_role_str = kParticipantRoleMemberStr;
		if (request_json.HasMember("role"))
		{
			if (request_json["role"].IsString())
			{
				target_role_str = request_json["role"].As<std::string>();
				if (target_role_str != kParticipantRoleMemberStr &&
					target_role_str != kParticipantRoleAdminStr)
				{
					throw userver::server::handlers::ClientError(
						userver::server::handlers::ExternalBody{
							R"({"code":"VALIDATION_ERROR","message":"Invalid 'role' value. Allowed: 'member', 'admin'."})"});
				}
			}
			else if (!request_json["role"].IsNull())
			{
				throw userver::server::handlers::ClientError(
					userver::server::handlers::ExternalBody{
						R"({"code":"VALIDATION_ERROR","message":"'role' field must be a string or null."})"});
			}
		}

		if (current_user_id_val == target_user_id_val)
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"VALIDATION_ERROR","message":"Cannot add yourself."})"});
		}

		ConversationAccessDetails access_details =
			CheckConversationAndPermissions(
				conversation_id_val, current_user_id_val, target_user_id_val);

		if (!access_details.exists)
		{
			http_request.SetResponseStatus(
				userver::server::http::HttpStatus::kNotFound);
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"NOT_FOUND","message":"Conversation not found or you are not an active participant."})"});
		}
		if (access_details.conversation_type_str != kConversationTypeGroupStr)
		{
			http_request.SetResponseStatus(
				userver::server::http::HttpStatus::kBadRequest);
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"INVALID_OPERATION","message":"Participants can only be added to group conversations."})"});
		}
		if (!access_details.current_user_is_admin)
		{
			http_request.SetResponseStatus(
				userver::server::http::HttpStatus::kForbidden);
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"FORBIDDEN","message":"Only administrators can add participants."})"});
		}
		if (access_details.target_user_is_participant)
		{
			http_request.SetResponseStatus(
				userver::server::http::HttpStatus::kConflict);
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"ALREADY_PARTICIPANT","message":"The target user is already an active participant."})"});
		}

		userver::formats::json::ValueBuilder response_participant_builder(
			userver::formats::json::Type::kObject);

		auto trx = pg_cluster_->Begin(
			"trx", userver::storages::postgres::ClusterHostType::kMaster, {});
		try
		{
			auto upsert_result = pg_cluster_->Execute(
				userver::storages::postgres::ClusterHostType::kMaster,
				kUpsertParticipant, conversation_id_val, target_user_id_val,
				target_role_str

			);

			if (upsert_result.IsEmpty())
			{
				throw std::runtime_error(
					"DB error: Participant upsert failed unexpectedly.");
			}

			const auto& row = upsert_result.Front();
			response_participant_builder["user_id"] =
				row["user_id"].As<std::optional<int64_t>>();
			response_participant_builder["role"] =
				row["role"].As<std::optional<std::string>>();
			response_participant_builder["joined_at"] =
				std::optional<userver::utils::datetime::TimePointTz>(
					row["joined_at"]
						.As<userver::storages::postgres::TimePointTz>()
						.GetUnderlying());

			if (!row["last_read_message_id"].IsNull())
			{
				response_participant_builder["last_read_message_id"] =
					row["last_read_message_id"].As<std::optional<int64_t>>();
			}
			if (!row["notifications_enabled"].IsNull())
			{
				response_participant_builder["notifications_enabled"] =
					row["notifications_enabled"].As<std::optional<bool>>();
			}

			trx.Commit();

			LOG_INFO() << "User " << current_user_id_val
					   << (access_details.target_user_was_participant
							   ? " re-added "
							   : " added ")
					   << "user " << target_user_id_val << " to conversation "
					   << conversation_id_val << " with role "
					   << target_role_str;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR() << "Generic error during participant addition: "
						<< e.what();
			trx.Rollback();
			throw userver::server::handlers::InternalServerError(
				userver::server::handlers::ExternalBody{
					R"({"code":"INTERNAL_ERROR","message":"An unexpected error occurred"})"});
		}

		http_request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
		return response_participant_builder.ExtractValue();
	}

} // namespace chat_service::http_handlers