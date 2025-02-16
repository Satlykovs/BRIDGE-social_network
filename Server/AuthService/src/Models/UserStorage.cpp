
#include "Models/UserStorage.hpp"
#include "schemas/User.hpp"
#include "sql/include/sql_queries/sql_queries.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component.hpp>
#include <optional>


#include <userver/storages/postgres/io/date.hpp>


namespace auth_service
{
    UserStorage::UserStorage(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context) : ComponentBase(config, context), 
        pgCluster_(context.FindComponent<userver::components::Postgres>("auth-db").GetCluster()) 
        {
            pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, sql_queries::sql::kCreateTable);
        }
        

        std::optional<auth_service::models::User> UserStorage::FindUserByEmail(const std::string& email)
        {
            auto result = pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, sql_queries::sql::kFindUserByEmail, email);  

            if (!result.IsEmpty())
            {
                return std::optional<auth_service::models::User>(result.AsSingleRow<auth_service::models::User>(userver::storages::postgres::kRowTag));
            }
            return std::nullopt;
        }

        bool UserStorage::CreateUser(const std::string& first_name, const std::string& last_name, const std::string& email, const std::string& password_hash)
        {
            pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, sql_queries::sql::kCreateUser, first_name, last_name, email, password_hash);

            return true;
        }



        
}

