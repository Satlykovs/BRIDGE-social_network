#include <userver/clients/dns/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/storages/secdist/component.hpp>
#include <userver/storages/secdist/provider_component.hpp>
// #include <userver/kafka/producer_component.hpp>

#include "Controllers/AuthController.hpp"
#include "Managers/AuthManager.hpp"
#include "Managers/JwtManager.hpp"
#include "Repositories/AuthRepository.hpp"
// #include "Managers/KafkaProducer.hpp"

int main(int argc, char *argv[]) {

  auto componentList = userver::components::MinimalServerComponentList()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::clients::dns::Component>()
                            .Append<auth_service::repositories::AuthRepository>()
                            .Append<auth_service::managers::AuthManager>()
                            .Append<auth_service::repositories::JwtRepository>()
                            .Append<auth_service::managers::JwtManager>()
                            .Append<userver::components::Secdist>()
                            .Append<userver::components::DefaultSecdistProvider>()
                            // .Append<userver::kafka::ProducerComponent>()
                            // .Append<auth_service::managers::Producer>()
                            .Append<userver::components::Postgres>("user-db");
    auth_service::controllers::AddAuthController(componentList);

  return userver::utils::DaemonMain(argc, argv, componentList);
}