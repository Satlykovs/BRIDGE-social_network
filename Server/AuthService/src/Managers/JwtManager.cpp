#include "Managers/JwtManager.hpp"
#include <userver/components/component.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/logging/log.hpp>
#include <chrono>


namespace auth_service
{

JwtManager::JwtManager(const userver::components::ComponentConfig &config,
                       const userver::components::ComponentContext& context) : ComponentBase(config, context),
    secretKey_(config["secret-key"].As<std::string>()),
    tokenLifetime_(config["token-lifetime"].As<int>())
    {
        LOG_INFO() << "JwtManager initialized with token lifetime: " << tokenLifetime_.count() << " hours";
    }

std::string JwtManager::GenerateToken(int user_id, const std::string& email) const
{
    return jwt::create()
    .set_issuer("auth-service")
    .set_audience("Qt-client")
    .set_subject(std::to_string(user_id))
    .set_type("JWT")
    .set_payload_claim("email", jwt::claim(email))
    .set_expires_at(std::chrono::system_clock::now() + tokenLifetime_)
    .sign(jwt::algorithm::hs256{secretKey_});
}

std::optional<JwtPayload> JwtManager::VerifyToken(const std::string& token) const
{
    try
    {
        auto decoded_token = jwt::decode(token);

        auto verifier = jwt::verify()
                            .allow_algorithm(jwt::algorithm::hs256{secretKey_})
                            .with_issuer("auth-service")
                            .with_audience("Qt-client")
                            .leeway(0);
        verifier.verify(decoded_token);

        JwtPayload payload;
        payload.email = decoded_token.get_payload_claim("email").as_string();

        return payload;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR() << "Error verifying token: " << e.what();
        return std::nullopt;
    }
}

userver::yaml_config::Schema JwtManager::GetStaticConfigSchema()
{
    return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
        type: object
        description: JWT Manager component for token generation and verification
        additionalProperties: true
        properties:
            secret-key:
                type: string
                description: Secret key used to sign the JWT tokens
            token-lifetime-hours:
                type: integer
                description: Lifetime of the JWT tokens in hours
                minimum: 1
        )");
        
}

}