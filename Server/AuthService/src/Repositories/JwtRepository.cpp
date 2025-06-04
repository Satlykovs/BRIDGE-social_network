#include "Repositories/JwtRepository.hpp"
#include "sql/include/sql_queries/sql_queries.hpp"
#include <userver/storages/postgres/component.hpp>

#include <userver/components/component.hpp>


namespace auth_service::repositories
{
    JwtRepository::JwtRepository(const userver::components::ComponentConfig& config,
         const userver::components::ComponentContext& context) : ComponentBase(config, context),
         pgCluster_(context.FindComponent<userver::components::Postgres>("user-db").GetCluster()) 
        {

        }

    void JwtRepository::AddRefreshToken(int userId, const std::string& token, userver::storages::postgres::TimePointWithoutTz expTime)
    {
        pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, sql_queries::sql::kAddRefreshToken, userId, token, expTime);
    }

    bool JwtRepository::CheckRefreshToken(const std::string& token) const
    {
        auto res = pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, sql_queries::sql::kFindRefreshToken, token);

        return !res.IsEmpty();
    }

    void JwtRepository::DeleteRefreshToken(const std::string& token)
    {
        pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, sql_queries::sql::kDeleteRefreshToken, token);
    }
}