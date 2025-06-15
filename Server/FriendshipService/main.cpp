#include <userver/clients/dns/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include "Controllers/FriendshipController.hpp"
#include "Managers/FriendshipManager.hpp"
#include "Repositories/FriendshipRepository.hpp"
#include <JwtMiddleware.hpp>

int main(int argc, char* argv[]){

    auto component_list = userver::components::MinimalServerComponentList()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::clients::dns::Component>()
                            .Append<friendship_service::FriendshipRepository>()
                            .Append<friendship_service::FriendshipManager>()
                            .Append<jwt_middleware::JwtMiddlewareFactory>()
                            .Append<userver::components::Postgres>("friendship-db");
    friendship_service::AddFriendshipController(component_list);
    return userver::utils::DaemonMain(argc, argv, component_list);
}

