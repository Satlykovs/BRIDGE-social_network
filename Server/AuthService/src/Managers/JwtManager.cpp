#include "Managers/JwtManager.hpp"
#include "Repositories/JwtRepository.hpp"
#include <userver/components/component.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include <userver/logging/log.hpp>
#include <userver/utils/optionals.hpp>
#include <chrono>
#include  <userver/utils/datetime/timepoint_tz.hpp>


namespace auth_service::managers
{

JwtManager::JwtManager(const userver::components::ComponentConfig &config,
                       const userver::components::ComponentContext& context) : ComponentBase(config, context),
    secretKey_(config["secret-key"].As<std::string>()),
    accessTokenLifetime_(config["access-token-lifetime"].As<int>()),
    refreshTokenLifetime_(config["refresh-token-lifetime"].As<int>()),
    jwtRepository_(context.FindComponent<auth_service::repositories::JwtRepository>())
    {
        LOG_INFO() << "JwtManager initialized with token lifetime: " << accessTokenLifetime_.count() << " minutes";
    }



std::string JwtManager::CreateToken(int userId, std::chrono::system_clock::time_point expTime) const
{
    return jwt::create()
    .set_issuer("auth-service")
    .set_audience("Qt-client")
    .set_subject(std::to_string(userId))
    .set_type("JWT")
    .set_expires_at(expTime)
    .sign(jwt::algorithm::hs256{secretKey_});
}

std::pair<std::string, userver::utils::datetime::TimePointTz> JwtManager::GenerateAccessToken(int userId) const
{
    auto expTime = std::chrono::system_clock::now() + accessTokenLifetime_;
    return {CreateToken(userId, expTime), userver::utils::datetime::TimePointTz(expTime)};
}

std::pair<std::string, userver::utils::datetime::TimePointTz> JwtManager::GenerateRefreshToken(int userId) const
{
    auto expTime = std::chrono::system_clock::now() + refreshTokenLifetime_;
    std::string token = CreateToken(userId, expTime);

    jwtRepository_.AddRefreshToken(userId, token, userver::storages::postgres::TimePointWithoutTz(expTime));

    return {token, userver::utils::datetime::TimePointTz(expTime)};

}

bool JwtManager::VerifyToken(const std::string& token) const
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
        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR() << "Error verifying token: " << e.what();
        return false;
    }
}


std::pair<std::pair<std::string, userver::utils::datetime::TimePointTz>, std::pair<std::string, userver::utils::datetime::TimePointTz>>
 JwtManager::RefreshTokens(const std::string& refreshToken)
{
    if (!VerifyToken(refreshToken))
    {
        throw std::runtime_error("INVALID_TOKEN");
    }
    
    if (!jwtRepository_.CheckRefreshToken(refreshToken))
    {
        jwtRepository_.DeleteRefreshToken(refreshToken);
        throw std::runtime_error(fmt::format("TOKEN_EXPIRED"));
    }

    jwtRepository_.DeleteRefreshToken(refreshToken);
    int userId = std::stoi(jwt::decode(refreshToken).get_subject());

    auto accessToken = GenerateAccessToken(userId);
    auto newRefreshToken = GenerateRefreshToken(userId);

    return std::make_pair(accessToken, newRefreshToken);

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