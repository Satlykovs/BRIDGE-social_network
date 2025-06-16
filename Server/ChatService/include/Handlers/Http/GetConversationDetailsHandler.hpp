#pragma once

#include <userver/components/component_context.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <vector>

namespace chat_service::http_handlers
{

	class GetConversationDetailsHandler final
		: public userver::server::handlers::HttpHandlerJsonBase
	{
	   public:
		static constexpr std::string_view kName =
			"get-conversation-details-handler";

		GetConversationDetailsHandler(
			const userver::components::ComponentConfig& config,
			const userver::components::ComponentContext& context);

		using RequestType = userver::formats::json::Value;
		using ResponseType = userver::formats::json::Value;

		ResponseType HandleRequestJsonThrow(
			const userver::server::http::HttpRequest& http_request,
			const RequestType& request_json,
			userver::server::request::RequestContext& context) const override;

	   private:
		userver::storages::postgres::ClusterPtr pg_cluster_;

		bool IsUserActiveParticipant(int conversation_id,
									 int64_t current_user_id) const;
	};

} // namespace chat_service::http_handlers