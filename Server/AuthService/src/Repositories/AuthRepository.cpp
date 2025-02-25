
#include "Repositories/AuthRepository.hpp"
#include "schemas/User.hpp"
#include "sql/include/sql_queries/sql_queries.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/components/component.hpp>
#include <optional>


#include <userver/storages/postgres/io/date.hpp>


namespace auth_service
{
    AuthRepository::AuthRepository(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context) : ComponentBase(config, context), 
        pgCluster_(context.FindComponent<userver::components::Postgres>("auth-db").GetCluster()) 
        {
            pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, sql_queries::sql::kCreateTableUsers);
        }
        

        std::optional<auth_service::models::User> AuthRepository::FindUserByEmail(const std::string& email)
        {
            auto result = pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, sql_queries::sql::kFindUserByEmail, email);  

            if (!result.IsEmpty())
            {
                return std::optional<auth_service::models::User>(result.AsSingleRow<auth_service::models::User>(userver::storages::postgres::kRowTag));
            }
            return std::nullopt;
        }

        bool AuthRepository::CreateUser(const std::string& email, const std::string& passwordHash)
        {
            pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, sql_queries::sql::kCreateUser, email, passwordHash);

            return true;
        }



        
}

