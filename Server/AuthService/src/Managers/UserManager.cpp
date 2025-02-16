#include "Managers/UserManager.hpp"
#include <userver/components/component.hpp>
#include <bcrypt/BCrypt.hpp>
#include <optional>

namespace auth_service
{
    UserManager::UserManager(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context) : 
    ComponentBase(config, context), userStorage_(context.FindComponent<UserStorage>()) {}

    bool UserManager::RegisterUser(auth_service::models::UserRegistrationDTO& user_data)
    {
        if (userStorage_.FindUserByEmail(user_data.email) != std::nullopt)
        {
            return false;
        }

        const std::string password_hash = HashPassword(user_data.password);

        return userStorage_.CreateUser(user_data.first_name, user_data.last_name, user_data.email, password_hash);;
        

        
    }


    std::string HashPassword(const std::string& password)
    {
        return BCrypt::generateHash(password);
    }

    bool VerifyPassword(const std::string& password, const std::string& hash)
    {
        return BCrypt::validatePassword(password, hash);
    }
}