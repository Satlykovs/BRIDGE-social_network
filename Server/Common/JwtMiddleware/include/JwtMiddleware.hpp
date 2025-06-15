#ifndef JWT_MIDDLEWARE_HPP
#define JWT_MIDDLEWARE_HPP

#include <userver/server/middlewares/http_middleware_base.hpp>
#include <userver/components/component_fwd.hpp>
#include <userver/yaml_config/schema.hpp>
#include <string>
#include <optional>

namespace jwt_middleware
{
    class JwtMiddleware final : public userver::server::middlewares::HttpMiddlewareBase
    {
        public:
            static constexpr std::string_view kName = "jwt-middleware";

            JwtMiddleware(std::string secretKey);
        private:
            void HandleRequest(userver::server::http::HttpRequest& request,
                               userver::server::request::RequestContext& context) const override;
            
            
            bool IsAuthPath(const std::string& path) const;
            std::string ExtractUserId(const std::string& token) const;
            bool VerifyToken(const std::string& token) const;

            

            const std::string secretKey_;
    };


    class JwtMiddlewareFactory final : public userver::server::middlewares::HttpMiddlewareFactoryBase
    {
        public:
            static constexpr std::string_view kName = JwtMiddleware::kName;

            JwtMiddlewareFactory(const userver::components::ComponentConfig& config, 
                                 const userver::components::ComponentContext& context);

            static userver::yaml_config::Schema GetStaticConfigSchema();


        private:
            std::unique_ptr<userver::server::middlewares::HttpMiddlewareBase> Create(
                const userver::server::handlers::HttpHandlerBase&, userver::yaml_config::YamlConfig) const;

            const std::string secretKey_;


    };

}


#endif