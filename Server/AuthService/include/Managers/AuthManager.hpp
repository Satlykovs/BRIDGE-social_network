#ifndef AUTH_MANAGER_HPP
#define AUTH_MANAGER_HPP

#include "Repositories/AuthRepository.hpp"
#include "Managers/JwtManager.hpp"
#include <userver/components/component_fwd.hpp>
#include <bcrypt/BCrypt.hpp>
#include <optional>
namespace auth_service
{

    class AuthManager final : public userver::components::ComponentBase
    {
        public:
            static constexpr std::string_view kName = "auth-manager";

            AuthManager(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            bool RegisterUser(auth_service::models::UserDTO& user_data);

            std::string AuthenticateUser(auth_service::models::UserDTO& user_data);

        private:
            auth_service::AuthRepository& authRepository_;
            auth_service::JwtManager& jwtManager_;
    };

    std::string HashPassword(const std::string& password);

    bool VerifyPassword(const std::string& password, const std::string& hash);
    bool IsValidPassword(const std::string& password);
    void ValidatePassword(const std::string& password);

}


#endif