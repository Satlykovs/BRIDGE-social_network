#ifndef PROFILE_CONTROLLER_HPP
#define PROFILE_CONTROLLER_HPP

#include "Managers/ProfileManager.hpp"

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/components/component_fwd.hpp>
#include <userver/s3api/clients/fwd.hpp>
#include <userver/http/common_headers.hpp>
#include <userver/formats/json/value.hpp>

namespace profile_service::controllers
{
    class UpdateAvatarHandler final : public userver::server::handlers::HttpHandlerBase
    {
        public:
            static constexpr std::string_view kName = "update-avatar-handler";

            UpdateAvatarHandler(const userver::components::ComponentConfig& config, 
                const userver::components::ComponentContext& context);

                std::string HandleRequestThrow(const userver::server::http::HttpRequest& request,
                    userver::server::request::RequestContext& context) const override;

        private:
                profile_service::managers::ProfileManager& profileManager_;
               
    };

    class GetInfoHandler final : public userver::server::handlers::HttpHandlerBase
    {
        public:
            static constexpr std::string_view kName = "get-info-handler";

            GetInfoHandler(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& context);

            std::string HandleRequestThrow(const userver::server::http::HttpRequest& request,
                userver::server::request::RequestContext& context) const override;

        private:
                profile_service::managers::ProfileManager& profileManager_;
    };

    
}



#endif