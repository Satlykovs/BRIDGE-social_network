#ifndef S3MANAGER_HPP
#define S3MANAGER_HPP

#include <userver/s3api/clients/fwd.hpp>
#include <userver/components/component_fwd.hpp>
#include <userver/components/component_base.hpp>
#include  <userver/clients/http/client.hpp>
#include  <userver/yaml_config/schema.hpp>

namespace profile_service::managers
{
    class S3Manager final : public userver::components::ComponentBase
    {
        public:
            static constexpr std::string_view kName = "s3-manager";

            S3Manager(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);


            userver::s3api::ClientPtr GetClient();

            static userver::yaml_config::Schema GetStaticConfigSchema();

            private:
                userver::clients::http::Client& httpClient_;
                std::string accessKey_;
                std::string secretKey_;
    };
}

#endif