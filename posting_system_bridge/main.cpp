#include <userver/clients/dns/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "include/Controllers/PostController.hpp"
#include "include/Managers/PostManager.hpp"
#include "include/Repositories/PostRepository.hpp"

int main(int argc, char *argv[]) {

    auto component_list = userver::components::MinimalServerComponentList()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::clients::dns::Component>()
                            .Append<post_service::PostStorage>()
                            .Append<post_service::PostManager>()
                            .Append<userver::components::Postgres>("post-db")
                            .Append<post_service::PostHandler>();
                            .Append<post_service::CommentStorage>()
                            .Append<post_service::CommentManager>()
                            .Append<post_service::CommentHandler>()

    return userver::utils::DaemonMain(argc, argv, component_list);
}
