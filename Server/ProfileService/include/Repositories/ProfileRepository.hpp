#ifndef PROFILE_REPOSITORY_HPP
#define PROFILE_REPOSITORY_HPP

#include "schemas/Profile.hpp"

#include <optional>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/components/component_base.hpp>


namespace profile_service::repositories
{
    class ProfileRepository final : public userver::components::ComponentBase
    {
        public:
            static constexpr std::string_view kName = "profile-repository";

            ProfileRepository(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            std::string UpdateAvatar(int id, const std::string& fileUrl);

            std::optional<profile_service::models::ProfileInfo> GetProfileById(int id);

        private:
            userver::storages::postgres::ClusterPtr pgCluster_;
    };
}

#endif