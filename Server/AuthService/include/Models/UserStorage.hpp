#ifndef USER_STORAGE_HPP
#define USER_STORAGE_HPP

#include "schemas/User.hpp"

#include <userver/components/component_fwd.hpp> 
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include <optional>

namespace auth_service
{
    class UserStorage final : public userver::components::ComponentBase
    {
        public:
            static constexpr std::string_view kName = "user-storage";

            UserStorage(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            std::optional<auth_service::models::User> FindUserByEmail(const std::string& email);
            bool CreateUser(const std::string& first_name, const std::string& last_name, const std::string& email, const std::string& password_hash);

        private:
            userver::storages::postgres::ClusterPtr pgCluster_;
    };
}



#endif