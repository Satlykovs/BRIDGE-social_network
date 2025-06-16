#include "Handlers/Http/SendMessageHandler.hpp"

#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/storages/postgres/options.hpp>
#include <userver/utils/datetime.hpp>
#include <userver/utils/datetime/timepoint_tz.hpp>
#include <userver/utils/from_string.hpp>
namespace chat_service::http_handlers
{

	namespace
	{
		const userver::storages::postgres::Query kInsertMessage{
			"INSERT INTO messages (conversation_id, sender_id, content, "
			"reply_to_id, "
			"attachments) "
			"VALUES ($1, $2, $3, $4, $5) "
			"RETURNING id, conversation_id, sender_id, content, attachments, "
			"reply_to_id, sent_at, edited_at",
			userver::storages::postgres::Query::Name{"insert_message_v3"},
		};

		const userver::storages::postgres::Query kUpdateConversationTimestamp{
			"UPDATE conversations SET last_message_id = $1, updated_at = NOW() "
			"WHERE "
			"id = $2",
			userver::storages::postgres::Query::Name{
				"update_conversation_timestamp_v3"},
		};

		const userver::storages::postgres::Query kCheckParticipantSendMessage{
			"SELECT 1 FROM participants WHERE conversation_id = $1 AND user_id "
			"= $2 "
			"AND left_at IS NULL LIMIT 1",
			userver::storages::postgres::Query::Name{
				"check_participant_send_message_v2"},
		};
	} // namespace

	SendMessageHandler::SendMessageHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& component_context)
		: HttpHandlerJsonBase(config, component_context),
		  pg_cluster_(
			  component_context
				  .FindComponent<userver::components::Postgres>("chat-db")
				  .GetCluster())
	{
	}

	bool SendMessageHandler::IsUserParticipant(int conversation_id,
											   int64_t user_id) const
	{
		auto result = pg_cluster_->Execute(
			userver::storages::postgres::ClusterHostType::kSlave,
			kCheckParticipantSendMessage, conversation_id, user_id);
		return !result.IsEmpty();
	}

	SendMessageHandler::ResponseType SendMessageHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& http_request,
		const RequestType& request_json,
		userver::server::request::RequestContext& request_context) const
	{
		int64_t sender_id_val;
		try
		{
			sender_id_val = request_context.GetData<int64_t>("user_id");
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

		if (!request_json.HasMember("content") ||
			!request_json["content"].IsString())
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"VALIDATION_ERROR","message":"'content' field is required and must be a string."})"});
		}
		std::string content_str = request_json["content"].As<std::string>();

		std::optional<int64_t> reply_to_id_opt;
		if (request_json.HasMember("reply_to_id"))
		{
			if (request_json["reply_to_id"].IsInt64())
			{
				reply_to_id_opt = request_json["reply_to_id"].As<int64_t>();
			}
			else if (!request_json["reply_to_id"].IsNull())
			{
				throw userver::server::handlers::ClientError(
					userver::server::handlers::ExternalBody{
						R"({"code":"VALIDATION_ERROR","message":"'reply_to_id' field must be an integer or null."})"});
			}
		}

		if (!IsUserParticipant(conversation_id_val, sender_id_val))
		{
			http_request.SetResponseStatus(
				userver::server::http::HttpStatus::kForbidden);
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"FORBIDDEN_OR_NOT_FOUND","message":"User is not a participant of this conversation or has left, or conversation not found."})"});
		}

		if (content_str.empty())
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"VALIDATION_ERROR","message":"Message content cannot be empty"})"});
		}

		auto trx = pg_cluster_->Begin(
			"trx", userver::storages::postgres::ClusterHostType::kMaster, {});
		userver::formats::json::ValueBuilder response_message_builder(
			userver::formats::json::Type::kObject);

		try
		{
			userver::formats::json::Value attachments_json =
				userver::formats::json::ValueBuilder(
					userver::formats::json::Type::kArray)
					.ExtractValue();

			auto msg_result =
				trx.Execute(kInsertMessage, conversation_id_val, sender_id_val,
							content_str, reply_to_id_opt, attachments_json

				);

			if (msg_result.IsEmpty())
			{
				throw std::runtime_error(
					"DB error: Message insertion failed unexpectedly.");
			}
			const auto& row = msg_result.Front();

			response_message_builder["id"] = row["id"].As<int64_t>();
			response_message_builder["conversation_id"] =
				row["conversation_id"].As<std::optional<int>>();
			response_message_builder["sender_id"] =
				row["sender_id"].As<std::optional<int64_t>>();
			if (!row["content"].IsNull())
			{
				response_message_builder["content"] =
					row["content"].As<std::optional<std::string>>();
			}
			response_message_builder["attachments"] =
				row["attachments"].As<userver::formats::json::Value>();

			if (!row["reply_to_id"].IsNull())
			{
				response_message_builder["reply_to_id"] =
					row["reply_to_id"].As<std::optional<int64_t>>();
			}
			response_message_builder["sent_at"] =
				std::optional<userver::utils::datetime::TimePointTz>(
					row["sent_at"]
						.As<userver::storages::postgres::TimePointTz>()
						.GetUnderlying());
			if (!row["edited_at"].IsNull())
			{
				response_message_builder["edited_at"] =
					std::optional<userver::utils::datetime::TimePointTz>(
						row["edited_at"]
							.As<userver::storages::postgres::TimePointTz>()
							.GetUnderlying());
			}

			trx.Execute(kUpdateConversationTimestamp,
						row["id"].As<std::optional<int64_t>>(),
						conversation_id_val);

			trx.Commit();
			LOG_INFO() << "User " << sender_id_val << " sent message "
					   << row["id"].As<int64_t>() << " to conversation "
					   << conversation_id_val;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR() << "Generic error during message sending: " << e.what();
			trx.Rollback();
			throw userver::server::handlers::InternalServerError(
				userver::server::handlers::ExternalBody{
					R"({"code":"INTERNAL_ERROR","message":"An unexpected error occurred"})"});
		}

		http_request.SetResponseStatus(
			userver::server::http::HttpStatus::kCreated);
		return response_message_builder.ExtractValue();
	}

} // namespace chat_service::http_handlers