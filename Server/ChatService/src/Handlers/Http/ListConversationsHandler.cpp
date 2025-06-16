#include "Handlers/Http/ListConversationsHandler.hpp"

#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/logging/log.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/storages/postgres/options.hpp>
#include <userver/utils/datetime.hpp>
#include <userver/utils/datetime/timepoint_tz.hpp>
namespace chat_service::http_handlers
{

	namespace
	{
		const userver::storages::postgres::Query kListUserConversations{
			"SELECT c.id, c.type, c.title, c.last_message_id, c.created_at, "
			"c.updated_at "
			"FROM conversations c "
			"JOIN participants p ON c.id = p.conversation_id "
			"WHERE p.user_id = $1 AND p.left_at IS NULL AND c.deleted = FALSE "
			"ORDER BY c.updated_at DESC",
			userver::storages::postgres::Query::Name{
				"list_user_conversations_v4"},
		};

	}

	ListConversationsHandler::ListConversationsHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& component_context)
		: HttpHandlerJsonBase(config, component_context),
		  pg_cluster_(
			  component_context
				  .FindComponent<userver::components::Postgres>("chat-db")
				  .GetCluster())
	{
	}

	ListConversationsHandler::ResponseType
	ListConversationsHandler::HandleRequestJsonThrow(
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

		userver::formats::json::ValueBuilder response_array_builder(
			userver::formats::json::Type::kArray);

		try
		{
			auto pg_result = pg_cluster_->Execute(
				userver::storages::postgres::ClusterHostType::kSlave,
				kListUserConversations, current_user_id_val);

			for (const auto& row : pg_result)
			{
				userver::formats::json::ValueBuilder conv_builder(
					userver::formats::json::Type::kObject);
				conv_builder["id"] = row["id"].As<int>();
				conv_builder["type"] = row["type"].As<std::string>();

				if (!row["title"].IsNull())
				{
					conv_builder["title"] = row["title"].As<std::string>();
				}
				if (!row["last_message_id"].IsNull())
				{
					conv_builder["last_message_id"] =
						row["last_message_id"].As<int64_t>();
				}
				conv_builder["created_at"] =
					std::optional<userver::utils::datetime::TimePointTz>(
						row["created_at"]
							.As<userver::storages::postgres::TimePointTz>()
							.GetUnderlying());
				conv_builder["updated_at"] =
					std::optional<userver::utils::datetime::TimePointTz>(
						row["updated_at"]
							.As<userver::storages::postgres::TimePointTz>()
							.GetUnderlying());

				response_array_builder.PushBack(conv_builder.ExtractValue());
			}
		}
		catch (const std::exception& e)
		{
			LOG_ERROR() << "Generic error while listing conversations: "
						<< e.what();
			throw userver::server::handlers::InternalServerError(
				userver::server::handlers::ExternalBody{
					R"({"code":"INTERNAL_ERROR","message":"An unexpected error occurred"})"});
		}

		http_request.SetResponseStatus(userver::server::http::HttpStatus::kOk);
		return response_array_builder.ExtractValue();
	}

} // namespace chat_service::http_handlers