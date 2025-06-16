
#include "Repositories/AuthRepository.hpp"

#include <optional>
#include <userver/components/component.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/date.hpp>

#include "schemas/User.hpp"
#include "sql/include/sql_queries/sql_queries.hpp"

namespace auth_service::repositories
{
	AuthRepository::AuthRepository(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: ComponentBase(config, context),
		  pgCluster_(
			  context.FindComponent<userver::components::Postgres>("user-db")
				  .GetCluster())
	{
	}

	std::optional<auth_service::models::User> AuthRepository::FindUserByEmail(
		const std::string& email)
	{
		auto result = pgCluster_->Execute(
			userver::storages::postgres::ClusterHostType::kMaster,
			sql_queries::sql::kFindUserByEmail, email);

		if (!result.IsEmpty())
		{
			return std::optional<auth_service::models::User>(
				result.AsSingleRow<auth_service::models::User>(
					userver::storages::postgres::kRowTag));
		}
		return std::nullopt;
	}

	void AuthRepository::CreateUser(const std::string& email,
									const std::string& passwordHash)
	{
		auto trx =
			pgCluster_->Begin(userver::storages::postgres::Transaction::RW);
		int id = trx.Execute(sql_queries::sql::kCreateUser, email, passwordHash)
					 .AsSingleRow<int>();

		trx.Execute("INSERT INTO user_profile(user_id, email) VALUES($1, $2)",
					id, email);

		trx.Commit();
	}

} // namespace auth_service::repositories
