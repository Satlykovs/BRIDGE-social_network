#ifndef USER_MANAGER_HPP
#define USER_MANAGER_HPP

#include "Models/UserStorage.hpp"

#include <userver/components/component_fwd.hpp>
#include <bcrypt/BCrypt.hpp>

namespace auth_service
{

    class UserManager final : public userver::components::ComponentBase
    {
        public:
            static constexpr std::string_view kName = "user-manager";

            UserManager(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            bool RegisterUser(auth_service::models::UserRegistrationDTO& user_data);

        private:
            auth_service::UserStorage& userStorage_;
    };

    std::string HashPassword(const std::string& password);

    bool VerifyPassword(const std::string& password, const std::string& hash);
    bool IsValidPassword(const std::string& password);
    void ValidatePassword(const std::string& password);

}


#endif