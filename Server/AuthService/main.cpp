#include <userver/clients/dns/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "Controllers/AuthController.hpp"
#include "Managers/AuthManager.hpp"
#include "Managers/JwtManager.hpp"
#include "Repositories/AuthRepository.hpp"

int main(int argc, char *argv[]) {

  auto componentList = userver::components::MinimalServerComponentList()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::clients::dns::Component>()
                            .Append<auth_service::JwtRepository>()
                            .Append<auth_service::JwtManager>()
                            .Append<auth_service::AuthRepository>()
                            .Append<auth_service::AuthManager>()
                            .Append<userver::components::Postgres>("auth-db");
    auth_service::AddAuthController(componentList);

  return userver::utils::DaemonMain(argc, argv, componentList);
}