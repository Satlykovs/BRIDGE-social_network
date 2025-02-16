#ifndef USER_CONTROLLER_HPP
#define USER_CONTROLLER_HPP

#include "Managers/UserManager.hpp"
#include  <userver/components/component_fwd.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/formats/json.hpp>

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
                auth_service::UserManager& userManager_;
    };
}

#endif