#include "Handlers/Http/UpdateMessageHandler.hpp"

#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/storages/postgres/options.hpp>
#include <userver/storages/postgres/transaction.hpp>
#include <userver/utils/datetime.hpp>
#include <userver/utils/datetime/timepoint_tz.hpp>
#include <userver/utils/from_string.hpp>
namespace chat_service::http_handlers
{

	namespace
	{
		const userver::storages::postgres::Query kGetMessageDetailsForUpdate{
			"SELECT conversation_id, sender_id, deleted FROM messages WHERE id "
			"= $1",
			userver::storages::postgres::Query::Name{
				"get_message_details_for_update_v2"},
		};

		const userver::storages::postgres::Query kUpdateMessageContent{
			"UPDATE messages SET content = $1, edited_at = NOW() "
			"WHERE id = $2 "
			"RETURNING id, conversation_id, sender_id, content, attachments, "
			"reply_to_id, sent_at, edited_at",
			userver::storages::postgres::Query::Name{
				"update_message_content_v3"},
		};

		const userver::storages::postgres::Query
			kUpdateConversationTimestampOnEdit{
				"UPDATE conversations SET updated_at = NOW() WHERE id = $1",
				userver::storages::postgres::Query::Name{
					"update_conversation_timestamp_on_edit_v3"},
			};
	} // namespace

	UpdateMessageHandler::UpdateMessageHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& component_context)
		: HttpHandlerJsonBase(config, component_context),
		  pg_cluster_(
			  component_context
				  .FindComponent<userver::components::Postgres>("chat-db")
				  .GetCluster())
	{
	}

	UpdateMessageHandler::MessageDetails
	UpdateMessageHandler::GetMessageDetailsAndCheckPermissions(
		int64_t message_id, int64_t current_user_id) const
	{
		auto result = pg_cluster_->Execute(
			userver::storages::postgres::ClusterHostType::kSlave,
			kGetMessageDetailsForUpdate, message_id);

		if (result.IsEmpty())
		{
			return {0, 0, false, false};
		}

		const auto row = result.Front();
		int conversation_id_val = row["conversation_id"].As<int>();
		int64_t sender_id_val = row["sender_id"].As<int64_t>();
		bool deleted = row["deleted"].As<bool>();

		if (sender_id_val != current_user_id)
		{
			return {0, 0, false, false};
		}
		return {conversation_id_val, sender_id_val, true, deleted};
	}

	UpdateMessageHandler::ResponseType
	UpdateMessageHandler::HandleRequestJsonThrow(
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

		const auto& message_id_str = http_request.GetPathArg("message_id");
		int64_t message_id_val;
		try
		{
			message_id_val =
				userver::utils::FromString<int64_t>(message_id_str);
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"INVALID_ARGUMENT","message":"Invalid message_id format"})"});
		}

		if (!request_json.HasMember("content") ||
			!request_json["content"].IsString())
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"VALIDATION_ERROR","message":"'content' field is required and must be a string."})"});
		}
		std::string content_str = request_json["content"].As<std::string>();

		if (content_str.empty())
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"VALIDATION_ERROR","message":"Message content cannot be empty"})"});
		}

		MessageDetails message_details = GetMessageDetailsAndCheckPermissions(
			message_id_val, current_user_id_val);

		if (!message_details.exists)
		{
			http_request.SetResponseStatus(
				userver::server::http::HttpStatus::kNotFound);
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"NOT_FOUND","message":"Message not found or permission denied."})"});
		}
		if (message_details.deleted)
		{
			http_request.SetResponseStatus(
				userver::server::http::HttpStatus::kBadRequest);
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"MESSAGE_DELETED","message":"Cannot edit a deleted message."})"});
		}

		auto trx = pg_cluster_->Begin(
			"trx", userver::storages::postgres::ClusterHostType::kMaster, {});
		userver::formats::json::ValueBuilder response_message_builder(
			userver::formats::json::Type::kObject);

		try
		{
			auto update_result =
				trx.Execute(kUpdateMessageContent, content_str, message_id_val);

			if (update_result.IsEmpty())
			{
				throw std::runtime_error(
					"DB error: Message update failed unexpectedly.");
			}

			const auto& row = update_result.Front();
			response_message_builder["id"] = row["id"].As<int64_t>();
			response_message_builder["conversation_id"] =
				row["conversation_id"].As<int>();
			response_message_builder["sender_id"] =
				row["sender_id"].As<int64_t>();
			if (!row["content"].IsNull())
			{
				response_message_builder["content"] =
					row["content"].As<std::string>();
			}
			response_message_builder["attachments"] =
				row["attachments"].As<userver::formats::json::Value>();
			if (!row["reply_to_id"].IsNull())
			{
				response_message_builder["reply_to_id"] =
					row["reply_to_id"].As<int64_t>();
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

			if (message_details.conversation_id_val > 0)
			{
				trx.Execute(kUpdateConversationTimestampOnEdit,
							message_details.conversation_id_val);
			}

			trx.Commit();
			LOG_INFO() << "User " << current_user_id_val << " updated message "
					   << message_id_val;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR() << "Generic error during message update: " << e.what();
			trx.Rollback();
			throw userver::server::handlers::InternalServerError(
				userver::server::handlers::ExternalBody{
					R"({"code":"INTERNAL_ERROR","message":"An unexpected error occurred"})"});
		}

		http_request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
		return response_message_builder.ExtractValue();
	}

} // namespace chat_service::http_handlers