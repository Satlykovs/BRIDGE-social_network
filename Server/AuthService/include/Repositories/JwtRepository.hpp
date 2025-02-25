#ifndef JWT_REPOSITORY_HPP
#define JWT_REPOSITORY_HPP

#include <userver/components/component_fwd.hpp> 
#include <userver/storages/postgres/cluster.hpp>
#include <userver/components/component_base.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
namespace auth_service
{
    class JwtRepository final : public userver::components::ComponentBase
    {
        public:
            static constexpr std::string_view kName = "jwt-repository";

            JwtRepository(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            void AddRefreshToken(int userId, const std::string& token, userver::storages::postgres::TimePointWithoutTz& expTime);
            bool CheckRefreshToken(const std::string& token) const;
            void DeleteRefreshToken(const std::string& token);

        private:
            userver::storages::postgres::ClusterPtr pgCluster_;
    };
}


#endif