#include "Managers/AuthManager.hpp"
#include "Managers/JwtManager.hpp"
#include <userver/components/component.hpp>
#include <bcrypt/BCrypt.hpp>
#include <optional>
#include <string>
namespace auth_service
{
    AuthManager::AuthManager(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context) : 
    ComponentBase(config, context), authRepository_(context.FindComponent<AuthRepository>()), jwtManager_(context.FindComponent<JwtManager>()) {}

    bool AuthManager::RegisterUser(auth_service::models::UserDTO& user_data)
    {
        if (authRepository_.FindUserByEmail(user_data.email) != std::nullopt)
        {
            return false;
        }

        ValidatePassword(user_data.password);
        const std::string password_hash = HashPassword(user_data.password);

        return authRepository_.CreateUser(user_data.email, password_hash);;
        
    }

    std::string AuthManager::AuthenticateUser(auth_service::models::UserDTO& user_data)
    {
        auto user = authRepository_.FindUserByEmail(user_data.email);
        if (user == std::nullopt)
        {
            throw std::runtime_error(fmt::format("User {} not found.", user_data.email));
        }

        ValidatePassword(user_data.password);
        
        if (!VerifyPassword(user_data.password, user.value().password_hash))
        {
            throw std::runtime_error("Invalid password");
        }
        return jwtManager_.GenerateToken(user.value().id, user.value().email);
        
    }


    std::string HashPassword(const std::string& password)
    {
        return BCrypt::generateHash(password);
    }

    bool VerifyPassword(const std::string& password, const std::string& hash)
    {
        return BCrypt::validatePassword(password, hash);
    }

    bool IsValidPassword(const std::string& password)
    {
        if (password.size() < 8)
        {
            return false;
        }

        bool has_upper = false;
        bool has_lower = false;
        bool has_digit = false;
        bool has_special = false;

        const std::string special_characters = "#?!@$%^&*-";

        for (char c : password) {
            if (std::isupper(c)) 
            {
                has_upper = true;
            } else if (std::islower(c)) 
            {
                has_lower = true;
            } else if (std::isdigit(c)) 
            {
                has_digit = true;
            } else if (special_characters.find(c) != std::string::npos) 
            {
                has_special = true;
            }
        }

        return has_upper && has_lower && has_digit && has_special;
    }
    
    void ValidatePassword(const std::string& password) {
        if (!IsValidPassword(password)) {
            throw std::invalid_argument("Password does not meet the requirements. "
                                        "It must be at least 8 characters long and contain "
                                        "at least one uppercase letter, one lowercase letter, "
                                        "one digit, and one special character from the set: #?!@$%^&*-");
        }
    }
}