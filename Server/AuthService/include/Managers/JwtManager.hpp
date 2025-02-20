#ifndef JWT_MANAGER_HPP
#define JWT_MANAGER_HPP

#include <jwt-cpp/jwt.h>
#include <userver/components/component_fwd.hpp>
#include <userver/components/component_base.hpp>
#include <userver/yaml_config/schema.hpp>
#include <optional>
#include <string>
#include <chrono>
#include "Models/JwtPayload.hpp"
namespace auth_service
{

class JwtManager final : public userver::components::ComponentBase
{
public:
    static constexpr std::string_view kName = "jwt-manager";

    JwtManager(const userver::components::ComponentConfig& config, 
                const userver::components::ComponentContext& context);
    
    std::string GenerateToken(int user_id, const std::string& email) const;

    std::optional<JwtPayload> VerifyToken(const std::string& token) const; //Note: Should be removed to Gateway in future
	
	static userver::yaml_config::Schema GetStaticConfigSchema();
    

private:
        std::string secretKey_;
        std::chrono::hours tokenLifetime_;

    
};
}
#endif