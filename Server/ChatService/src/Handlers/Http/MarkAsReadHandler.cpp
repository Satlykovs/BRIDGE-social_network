#include "Handlers/Http/MarkAsReadHandler.hpp"

#include <userver/formats/serialize/common_containers.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/options.hpp>
#include <userver/utils/from_string.hpp>

namespace chat_service::http_handlers
{

	namespace
	{
		const userver::storages::postgres::Query kUpdateLastReadMessage{
			"UPDATE participants "
			"SET last_read_message_id = $1 "
			"WHERE conversation_id = $2 AND user_id = $3 AND "
			"    ($1 > participants.last_read_message_id OR "
			"participants.last_read_message_id IS NULL)",
			userver::storages::postgres::Query::Name{
				"update_last_read_message_v2"},
		};

		const userver::storages::postgres::Query kCheckParticipantForMarkRead{
			"SELECT 1 FROM participants WHERE conversation_id = $1 AND user_id "
			"= $2 "
			"AND left_at IS NULL LIMIT 1",
			userver::storages::postgres::Query::Name{
				"check_participant_for_mark_read_v2"},
		};

		const userver::storages::postgres::Query
			kCheckMessageExistsInConversationForMarkRead{
				"SELECT 1 FROM messages WHERE id = $1 AND conversation_id = $2 "
				"AND "
				"deleted = FALSE LIMIT 1",
				userver::storages::postgres::Query::Name{
					"check_message_exists_for_mark_read_v2"},
			};
	} // namespace

	MarkAsReadHandler::MarkAsReadHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& component_context)
		: HttpHandlerJsonBase(config, component_context),
		  pg_cluster_(
			  component_context
				  .FindComponent<userver::components::Postgres>("chat-db")
				  .GetCluster())
	{
	}

	bool MarkAsReadHandler::IsUserParticipant(int conversation_id,
											  int64_t user_id) const
	{
		auto result = pg_cluster_->Execute(
			userver::storages::postgres::ClusterHostType::kSlave,
			kCheckParticipantForMarkRead, conversation_id, user_id);
		return !result.IsEmpty();
	}

	bool MarkAsReadHandler::IsValidMessageInConversation(
		int64_t message_id, int conversation_id) const
	{
		if (message_id <= 0) return false;
		auto result = pg_cluster_->Execute(
			userver::storages::postgres::ClusterHostType::kSlave,
			kCheckMessageExistsInConversationForMarkRead, message_id,
			conversation_id);
		return !result.IsEmpty();
	}

	MarkAsReadHandler::ResponseType MarkAsReadHandler::HandleRequestJsonThrow(
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
					"User not authenticated or user_id missing in context"});
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

		if (!request_json.HasMember("last_read_message_id") ||
			!request_json["last_read_message_id"].IsInt64())
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"VALIDATION_ERROR","message":"'last_read_message_id' field is required and must be an integer."})"});
		}
		int64_t last_read_message_id_val =
			request_json["last_read_message_id"].As<int64_t>();

		if (!IsUserParticipant(conversation_id_val, current_user_id_val))
		{
			http_request.SetResponseStatus(
				userver::server::http::HttpStatus::kForbidden);
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"FORBIDDEN_OR_NOT_FOUND","message":"Conversation not found or you are not a participant."})"});
		}

		if (!IsValidMessageInConversation(last_read_message_id_val,
										  conversation_id_val))
		{
			LOG_INFO() << "Attempt to mark non-existent or invalid message_id "
					   << last_read_message_id_val
					   << " as read in conversation " << conversation_id_val;
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"INVALID_ARGUMENT","message":"Provided last_read_message_id is invalid or does not belong to this conversation."})"});
		}

		auto trx = pg_cluster_->Begin(
			"trx", userver::storages::postgres::ClusterHostType::kMaster, {});
		try
		{
			auto result = pg_cluster_->Execute(

				userver::storages::postgres::ClusterHostType::kMaster,
				kUpdateLastReadMessage, last_read_message_id_val,
				conversation_id_val, current_user_id_val);

			if (result.RowsAffected() > 0)
			{
				LOG_INFO() << "User " << current_user_id_val
						   << " marked conversation " << conversation_id_val
						   << " as read up to message "
						   << last_read_message_id_val;
			}
			else
			{
				LOG_INFO() << "User " << current_user_id_val
						   << " tried to mark conversation "
						   << conversation_id_val << " as read up to message "
						   << last_read_message_id_val
						   << ", but no update was needed.";
			}
			trx.Commit();
		}
		catch (const std::exception& e)
		{
			LOG_ERROR() << "Generic error while marking message as read: "
						<< e.what();
			trx.Rollback();
			throw userver::server::handlers::InternalServerError(
				userver::server::handlers::ExternalBody{
					R"({"code":"INTERNAL_ERROR","message":"An unexpected error occurred"})"});
		}

		http_request.SetResponseStatus(
			userver::server::http::HttpStatus::kNoContent);
		return {};
	}

} // namespace chat_service::http_handlers