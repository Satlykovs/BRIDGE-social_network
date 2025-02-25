#include "Managers/JwtManager.hpp"
#include "Repositories/JwtRepository.hpp"
#include <userver/components/component.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/logging/log.hpp>
#include <chrono>


namespace auth_service
{

JwtManager::JwtManager(const userver::components::ComponentConfig &config,
                       const userver::components::ComponentContext& context) : ComponentBase(config, context),
    secretKey_(config["secret-key"].As<std::string>()),
    accessTokenLifetime_(config["access-token-lifetime"].As<int>()),
    refreshTokenLifetime_(config["refresh-token-lifetime"].As<int>()),
    jwtRepository_(context.FindComponent<JwtRepository>())
    {
        LOG_INFO() << "JwtManager initialized with token lifetime: " << accessTokenLifetime_.count() << " minutes";
    }

std::string JwtManager::GenerateAccessToken(int userId) const
{
    return jwt::create()
    .set_issuer("auth-service")
    .set_audience("Qt-client")
    .set_subject(std::to_string(userId))
    .set_type("JWT")
    .set_expires_at(std::chrono::system_clock::now() + accessTokenLifetime_)
    .sign(jwt::algorithm::hs256{secretKey_});
}

std::string JwtManager::GenerateRefreshToken(int userId) const
{
    auto expTime = userver::storages::postgres::TimePointWithoutTz(std::chrono::system_clock::now() + refreshTokenLifetime_);
    std::string token = jwt::create()
    .set_issuer("auth-service")
    .set_audience("Qt-client")
    .set_subject(std::to_string(userId))
    .set_expires_at(expTime)
    .sign(jwt::algorithm::hs256{secretKey_});

    jwtRepository_.AddRefreshToken(userId, token, expTime);

    return token;

}

std::optional<JwtPayload> JwtManager::VerifyAccessToken(const std::string& token) const
{
    try
    {
        auto decodedToken = jwt::decode(token);

        auto verifier = jwt::verify()
                            .allow_algorithm(jwt::algorithm::hs256{secretKey_})
                            .with_issuer("auth-service")
                            .with_audience("Qt-client")
                            .leeway(0);
        verifier.verify(decodedToken);

        JwtPayload payload;
        payload.email = decodedToken.get_payload_claim("email").as_string();

        return payload;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR() << "Error verifying token: " << e.what();
        return std::nullopt;
    }
}


std::pair<std::string, std::string> JwtManager::RefreshTokens(const std::string& refreshToken)
{
    if (!jwtRepository_.CheckRefreshToken(refreshToken))
    {
        jwtRepository_.DeleteRefreshToken(refreshToken);
        throw std::runtime_error(fmt::format("TOKEN_EXPIRED"));
    }

    jwtRepository_.DeleteRefreshToken(refreshToken);
    int userId = std::stoi(jwt::decode(refreshToken).get_subject());

    std::string accessToken = GenerateAccessToken(userId);
    std::string newRefreshToken = GenerateRefreshToken(userId);

    return {accessToken, newRefreshToken};

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
            access-token-lifetime:
                type: integer
                description: Lifetime of the JWT access tokens (in minutes)
                minimum: 1
            refresh-token-lifetime:
                type: integer
                description: Lifetime of the JWT refresh token (in hours)
                minimum: 1
        )");
        
}

}