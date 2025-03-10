#ifndef FRIEND_CONTROLLER_HPP
#define FRIEND_CONTROLLER_HPP

#include "Managers/FriendshipManager.hpp"
#include <userver/components/component_fwd.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/components/component_list.hpp>
#include <userver/formats/json.hpp>



namespace friendship_service
{

    class FriendListByTypeHandler final : public userver::server::handlers::HttpHandlerJsonBase
    {
        public:
            static constexpr std::string_view kName = "friend-list-by-type-handler";

            FriendListByTypeHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            userver::formats::json::Value HandleRequestJsonThrow(
                const userver::server::http::HttpRequest& request,
                const userver::formats::json::Value& request_json,
                userver::server::request::RequestContext& context) const override;
        private:
            friendship_service::FriendshipManager& FriendshipManager_;
    };


    class GetStatusFriendshipRelationHandler final : public userver::server::handlers::HttpHandlerJsonBase
    {
        public:
            static constexpr std::string_view kName = "get-status-friendship-relation-handler";

            GetStatusFriendshipRelationHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            userver::formats::json::Value HandleRequestJsonThrow(
                const userver::server::http::HttpRequest& request,
                const userver::formats::json::Value& request_json,
                userver::server::request::RequestContext& context) const override;
        private:
            friendship_service::FriendshipManager& FriendshipManager_;
    };


    class RemoveFriendHandler final : public userver::server::handlers::HttpHandlerJsonBase
    {
        public:
            static constexpr std::string_view kName = "remove-friend-handler";

            RemoveFriendHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            userver::formats::json::Value HandleRequestJsonThrow(
                const userver::server::http::HttpRequest& request,
                const userver::formats::json::Value& request_json,
                userver::server::request::RequestContext& context) const override;
        private:
            friendship_service::FriendshipManager& FriendshipManager_;
    };


    class RevokeFriendshipRequestHandler final : public userver::server::handlers::HttpHandlerJsonBase
    {
        public:
            static constexpr std::string_view kName = "revoke-friendship-request-handler";

            RevokeFriendshipRequestHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            userver::formats::json::Value HandleRequestJsonThrow(
                const userver::server::http::HttpRequest& request,
                const userver::formats::json::Value& request_json,
                userver::server::request::RequestContext& context) const override;
        private:
            friendship_service::FriendshipManager& FriendshipManager_;
    };


    class SendFriendshipRequestHandler final : public userver::server::handlers::HttpHandlerJsonBase
    {
        public:
            static constexpr std::string_view kName = "send-friendship-request-handler";

            SendFriendshipRequestHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            userver::formats::json::Value HandleRequestJsonThrow(
                const userver::server::http::HttpRequest& request,
                const userver::formats::json::Value& request_json,
                userver::server::request::RequestContext& context) const override;
        private:
            friendship_service::FriendshipManager& FriendshipManager_;
    };


    class FriendshipRequestDecisionHandler final : public userver::server::handlers::HttpHandlerJsonBase
    {
        public:
            static constexpr std::string_view kName = "friendship-request-decision-handler";

            FriendshipRequestDecisionHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            userver::formats::json::Value HandleRequestJsonThrow(
                const userver::server::http::HttpRequest& request,
                const userver::formats::json::Value& request_json,
                userver::server::request::RequestContext& context) const override;
        private:
            friendship_service::FriendshipManager& FriendshipManager_;
    };

    void AddFriendshipController(userver::components::ComponentList& componenr_list);
    
}

#endif