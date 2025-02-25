#ifndef AUTH_CONTROLLER_HPP
#define AUTH_CONTROLLER_HPP

#include "Managers/AuthManager.hpp"
#include  <userver/components/component_fwd.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/formats/json.hpp>
#include <userver/components/component_list.hpp>

namespace auth_service
{
    class RegisterHandler final : public userver::server::handlers::HttpHandlerJsonBase
    {
        public:
            static constexpr std::string_view kName = "register-handler";

            RegisterHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            userver::formats::json::Value HandleRequestJsonThrow(
                const userver::server::http::HttpRequest& request,
                const userver::formats::json::Value& request_json,
                userver::server::request::RequestContext& context) const;
        
            private:
                AuthManager& authManager_;
    };

    class LoginHandler final : public userver::server::handlers::HttpHandlerJsonBase
    {
        public:
            static constexpr std::string_view kName = "login-handler";

            LoginHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            userver::formats::json::Value HandleRequestJsonThrow(
                const userver::server::http::HttpRequest& request,
                const userver::formats::json::Value& request_json,
                userver::server::request::RequestContext& context) const;

            private:
                AuthManager& authManager_;
    };

    class RefreshHandler final : public userver::server::handlers::HttpHandlerJsonBase
    {
        public:
            static constexpr std::string_view kName = "refresh-handler";

            RefreshHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext&  context);

            userver::formats::json::Value HandleRequestJsonThrow(
                const userver::server::http::HttpRequest& request,
                const userver::formats::json::Value& request_json,
                userver::server::request::RequestContext& context) const;

            private:
                JwtManager& jwtManager_;
    };

    void AddAuthController(userver::components::ComponentList& component_list);


}

#endif