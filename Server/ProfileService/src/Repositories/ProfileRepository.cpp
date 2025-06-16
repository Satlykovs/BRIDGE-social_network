#include "Repositories/ProfileRepository.hpp"

#include <optional>
#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include "schemas/Profile.hpp"
#include "sql/include/sql_queries/sql_queries.hpp"

namespace profile_service::repositories
{
	ProfileRepository::ProfileRepository(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: ComponentBase(config, context),
		  pgCluster_(
			  context.FindComponent<userver::components::Postgres>("user-db")
				  .GetCluster())
	{
	}

	std::string ProfileRepository::UpdateAvatar(int id,
												const std::string& fileUrl)
	{
		return pgCluster_
			->Execute(userver::storages::postgres::ClusterHostType::kMaster,
					  sql_queries::sql::kUpdateUserAvatar, id, fileUrl)
			.AsSingleRow<std::string>();
	}

	std::optional<profile_service::models::ProfileInfo>
	ProfileRepository::GetProfileById(int id)
	{
		auto res = pgCluster_->Execute(
			userver::storages::postgres::ClusterHostType::kSlave,
			sql_queries::sql::kFindProfileById, id);

		if (res.IsEmpty())
		{
			return std::nullopt;
		}
		return res.AsSingleRow<profile_service::models::ProfileInfo>(
			userver::storages::postgres::kRowTag);
	}

	profile_service::models::ProfileInfo ProfileRepository::UpdateInfo(
		int id, const std::string& first_name, const std::string& last_name,
		const std::string& username)
	{
		return pgCluster_
			->Execute(userver::storages::postgres::ClusterHostType::kMaster,
					  sql_queries::sql::kUpdateUserProfile, id, first_name,
					  last_name, username)
			.AsSingleRow<profile_service::models::ProfileInfo>(
				userver::storages::postgres::kRowTag);
	}

} // namespace profile_service::repositories