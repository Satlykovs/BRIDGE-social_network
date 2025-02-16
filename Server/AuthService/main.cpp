#include <userver/components/minimal_server_component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/clients/dns/component.hpp>


#include "Controllers/UserController.hpp"
#include "Models/UserStorage.hpp"
#include "Managers/UserManager.hpp"


int main(int argc, char* argv[]) {

    auto component_list = userver::components::MinimalServerComponentList()
                                                .Append<userver::components::TestsuiteSupport>()
                                                .Append<userver::clients::dns::Component>()
                                                .Append<auth_service::RegisterHandler>()
                                                .Append<auth_service::UserStorage>()
                                                .Append<auth_service::UserManager>()
                                                .Append<userver::components::Postgres>("auth-db");
  
    return userver::utils::DaemonMain(argc, argv, component_list);
}