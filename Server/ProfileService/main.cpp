#include "Controllers/ProfileController.hpp"
#include "Managers/S3Manager.hpp"
#include "Managers/ProfileManager.hpp"
#include "Repositories/ProfileRepository.hpp"
#include <JwtMiddleware.hpp>

#include <userver/components/minimal_server_component_list.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/clients/dns/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
#include <userver/utils/daemon_run.hpp>



int main(int argc, char *argv[])

{
    auto componentList = userver::components::MinimalServerComponentList()
                                            .Append<userver::components::HttpClient>()
                                            .Append<userver::clients::dns::Component>()
                                            .Append<profile_service::controllers::UpdateAvatarHandler>()
                                            .Append<profile_service::controllers::InfoHandler>()
                                            .Append<profile_service::managers::S3Manager>()
                                            .Append<profile_service::managers::ProfileManager>()
                                            .Append<profile_service::repositories::ProfileRepository>()
                                            .Append<userver::components::Postgres>("user-db")
                                            .Append<userver::components::Secdist>()
                                            .Append<userver::components::DefaultSecdistProvider>()
                                            .Append<userver::components::TestsuiteSupport>()
                                            .Append<jwt_middleware::JwtMiddlewareFactory>();

    return userver::utils::DaemonMain(argc, argv, componentList);
}