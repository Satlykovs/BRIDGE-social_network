#ifndef AUTH_MANAGER_HPP
#define AUTH_MANAGER_HPP

#include "Repositories/AuthRepository.hpp"
#include "Managers/JwtManager.hpp"
// #include "Managers/KafkaProducer.hpp"
#include <userver/components/component_fwd.hpp>
#include <bcrypt/BCrypt.hpp>
#include <optional>
#include <userver/formats/json/value.hpp>


namespace auth_service::managers
{

    class AuthManager final : public userver::components::ComponentBase
    {
        public:
            static constexpr std::string_view kName = "auth-manager";

            AuthManager(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            void RegisterUser(auth_service::models::UserDTO& user_data);

            std::pair<std::pair<std::string, userver::utils::datetime::TimePointTz>, std::pair<std::string, userver::utils::datetime::TimePointTz>>
            AuthenticateUser(auth_service::models::UserDTO& user_data);


        private:
            auth_service::repositories::AuthRepository& authRepository_;
            auth_service::managers::JwtManager& jwtManager_;
            //auth_service::managers::Producer& kafkaProducer_;
    };

    std::string HashPassword(const std::string& password);

    bool VerifyPassword(const std::string& password, const std::string& hash);
    bool IsValidPassword(const std::string& password);
    void ValidatePassword(const std::string& password);

}


#endif