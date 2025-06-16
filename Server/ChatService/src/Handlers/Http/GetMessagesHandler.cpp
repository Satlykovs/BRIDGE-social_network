#include "Handlers/Http/GetMessagesHandler.hpp"

#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/storages/postgres/options.hpp>
#include <userver/utils/assert.hpp>
#include <userver/utils/datetime.hpp>
#include <userver/utils/datetime/timepoint_tz.hpp>
#include <userver/utils/from_string.hpp>

namespace chat_service::http_handlers
{

	namespace
	{
		const std::string kListMessagesQueryBase =
			"SELECT id, conversation_id, sender_id, content, attachments, "
			"reply_to_id, "
			"sent_at, edited_at "
			"FROM messages "
			"WHERE conversation_id = $1 AND deleted = FALSE ";

		const userver::storages::postgres::Query
			kCheckParticipantForGetMessages{
				"SELECT 1 FROM participants WHERE conversation_id = $1 AND "
				"user_id = $2 "
				"AND left_at IS NULL LIMIT 1",
				userver::storages::postgres::Query::Name{
					"check_participant_for_get_messages_v2"},
			};
	} // namespace

	GetMessagesHandler::GetMessagesHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& component_context)
		: HttpHandlerJsonBase(config, component_context),
		  pg_cluster_(
			  component_context
				  .FindComponent<userver::components::Postgres>("chat-db")
				  .GetCluster())
	{
	}

	bool GetMessagesHandler::IsUserParticipant(int conversation_id,
											   int64_t user_id) const
	{
		auto result = pg_cluster_->Execute(
			userver::storages::postgres::ClusterHostType::kSlave,
			kCheckParticipantForGetMessages, conversation_id, user_id);
		return !result.IsEmpty();
	}

	GetMessagesHandler::ResponseType GetMessagesHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& http_request,
		const RequestType&,
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

		if (!IsUserParticipant(conversation_id_val, current_user_id_val))
		{
			http_request.SetResponseStatus(
				userver::server::http::HttpStatus::kForbidden);
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					R"({"code":"FORBIDDEN_OR_NOT_FOUND","message":"Conversation not found or you are not a participant."})"});
		}

		int limit = 50;
		if (http_request.HasArg("limit"))
		{
			try
			{
				limit = userver::utils::FromString<int>(
					std::string(http_request.GetArg("limit")));
				if (limit <= 0 || limit > 100) limit = 50;
			}
			catch (const std::exception& e)
			{
				LOG_WARNING()
					<< "Failed to parse 'limit' query_param: " << e.what()
					<< ". Using default 50.";
			}
		}

		std::optional<int64_t> before_message_id_val;
		if (http_request.HasArg("before_message_id"))
		{
			try
			{
				before_message_id_val = userver::utils::FromString<int64_t>(
					std::string(http_request.GetArg("before_message_id")));
			}
			catch (const std::exception& e)
			{
				LOG_WARNING()
					<< "Failed to parse 'before_message_id' query_param: "
					<< e.what();
				throw userver::server::handlers::ClientError(
					userver::server::handlers::ExternalBody{
						R"({"code":"INVALID_ARGUMENT","message":"Invalid before_message_id format"})"});
			}
		}

		std::string query_string = kListMessagesQueryBase;
		userver::storages::postgres::ParameterStore params;
		params.PushBack(conversation_id_val);

		if (before_message_id_val)
		{
			query_string +=
				"AND id < $" + std::to_string(params.Size() + 1) + " ";
			params.PushBack(*before_message_id_val);
		}
		query_string +=
			"ORDER BY id DESC LIMIT $" + std::to_string(params.Size() + 1);
		params.PushBack(limit);

		userver::formats::json::ValueBuilder response_array_builder(
			userver::formats::json::Type::kArray);
		try
		{
			auto pg_result = pg_cluster_->Execute(
				userver::storages::postgres::ClusterHostType::kSlave,
				userver::storages::postgres::Query{query_string}, params);

			for (const auto& row : pg_result)
			{
				userver::formats::json::ValueBuilder msg_builder(
					userver::formats::json::Type::kObject);
				msg_builder["id"] = row["id"].As<int64_t>();
				msg_builder["conversation_id"] =
					row["conversation_id"].As<int>();
				msg_builder["sender_id"] = row["sender_id"].As<int64_t>();
				if (!row["content"].IsNull())
				{
					msg_builder["content"] = row["content"].As<std::string>();
				}
				msg_builder["attachments"] =
					row["attachments"].As<userver::formats::json::Value>();
				if (!row["reply_to_id"].IsNull())
				{
					msg_builder["reply_to_id"] =
						row["reply_to_id"].As<int64_t>();
				}
				msg_builder["sent_at"] = userver::utils::datetime::Timestring(
					row["sent_at"].As<std::chrono::system_clock::time_point>());
				if (!row["edited_at"].IsNull())
				{
					msg_builder["edited_at"] =
						userver::utils::datetime::Timestring(
							row["edited_at"]
								.As<std::chrono::system_clock::time_point>());
				}
				response_array_builder.PushBack(msg_builder.ExtractValue());
			}
		}
		catch (const std::exception& e)
		{
			LOG_ERROR() << "Generic error while listing messages: " << e.what();
			throw userver::server::handlers::InternalServerError(
				userver::server::handlers::ExternalBody{
					R"({"code":"INTERNAL_ERROR","message":"An unexpected error occurred"})"});
		}

		http_request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
		return response_array_builder.ExtractValue();
	}

} // namespace chat_service::http_handlers