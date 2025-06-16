#ifndef AUTH_REPOSITORY_HPP
#define AUTH_REPOSITORY_HPP

#include <optional>
#include <userver/components/component_base.hpp>
#include <userver/components/component_fwd.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include "schemas/User.hpp"

namespace auth_service::repositories
{
	class AuthRepository final : public userver::components::ComponentBase
	{
	   public:
		static constexpr std::string_view kName = "auth-repository";

		AuthRepository(const userver::components::ComponentConfig& config,
					   const userver::components::ComponentContext& context);

		std::optional<auth_service::models::User> FindUserByEmail(
			const std::string& email);
		void CreateUser(const std::string& email,
						const std::string& password_hash);

	   private:
		userver::storages::postgres::ClusterPtr pgCluster_;
	};
} // namespace auth_service::repositories

#endif