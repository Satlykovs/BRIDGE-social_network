#ifndef PROFILE_REPOSITORY_HPP
#define PROFILE_REPOSITORY_HPP

#include <optional>
#include <userver/components/component_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include "schemas/Profile.hpp"

namespace profile_service::repositories
{
	class ProfileRepository final : public userver::components::ComponentBase
	{
	   public:
		static constexpr std::string_view kName = "profile-repository";

		ProfileRepository(const userver::components::ComponentConfig& config,
						  const userver::components::ComponentContext& context);

		std::string UpdateAvatar(int id, const std::string& fileUrl);

		profile_service::models::ProfileInfo UpdateInfo(
			int id, const std::string& first_name, const std::string& last_name,
			const std::string& username);

		std::optional<profile_service::models::ProfileInfo> GetProfileById(
			int id);

	   private:
		userver::storages::postgres::ClusterPtr pgCluster_;
	};
} // namespace profile_service::repositories

#endif