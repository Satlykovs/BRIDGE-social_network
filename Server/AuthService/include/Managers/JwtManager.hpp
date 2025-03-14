#ifndef JWT_MANAGER_HPP
#define JWT_MANAGER_HPP

#include <jwt-cpp/jwt.h>
#include <userver/components/component_fwd.hpp>
#include <userver/yaml_config/schema.hpp>

#include <optional>
#include <string>
#include <chrono>
#include  "Repositories/JwtRepository.hpp"
namespace auth_service::managers
{

class JwtManager final : public userver::components::ComponentBase
{
public:
    static constexpr std::string_view kName = "jwt-manager";

    JwtManager(const userver::components::ComponentConfig& config, 
                const userver::components::ComponentContext& context);
    
    std::string GenerateAccessToken(int userId) const;
    std::string GenerateRefreshToken(int userId) const;

    std::pair<std::string, std::string> RefreshTokens(const std::string& refreshToken);

    bool VerifyToken(const std::string& token) const; //Note: Should be removed to Gateway in future
	
	static userver::yaml_config::Schema GetStaticConfigSchema();
    

private:
        std::string secretKey_;
        std::chrono::minutes accessTokenLifetime_;
        std::chrono::hours refreshTokenLifetime_;

        std::string CreateToken(int userId, std::chrono::system_clock::time_point expTime) const;


        auth_service::repositories::JwtRepository& jwtRepository_;

    
};
}
#endif