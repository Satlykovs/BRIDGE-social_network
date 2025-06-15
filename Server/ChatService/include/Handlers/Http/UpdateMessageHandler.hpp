#pragma once

#include <userver/components/component_context.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace chat_service::http_handlers {

class UpdateMessageHandler final
    : public userver::server::handlers::HttpHandlerJsonBase {
public:
  static constexpr std::string_view kName = "update-message-handler";

  UpdateMessageHandler(const userver::components::ComponentConfig &config,
                       const userver::components::ComponentContext &context);

  using RequestType = userver::formats::json::Value;
  using ResponseType = userver::formats::json::Value;

  ResponseType HandleRequestJsonThrow(
      const userver::server::http::HttpRequest &http_request,
      const RequestType &request_json,
      userver::server::request::RequestContext &context) const override;

private:
  userver::storages::postgres::ClusterPtr pg_cluster_;

  struct MessageDetails {
    int conversation_id_val;
    int64_t sender_id_val;
    bool exists;
    bool deleted;
  };

  MessageDetails
  GetMessageDetailsAndCheckPermissions(int64_t message_id,
                                       int64_t current_user_id) const;
};

} // namespace chat_service::http_handlers