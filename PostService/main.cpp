#include "Managers/S3Manager.hpp"
#include "Managers/PostManager.hpp"
#include "Controllers/PostController.hpp"
#include "Repositories/PostRepository.hpp"



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
                                            .Append<userver::components::Postgres>("post-db")
                                            .Append<userver::components::Secdist>()
                                            .Append<userver::components::DefaultSecdistProvider>()
                                            .Append<userver::components::TestsuiteSupport>()
                                            .Append<jwt_middleware::JwtMiddlewareFactory>()
                                            .Append<post_service::controllers::CreatePostHandler>()
                                            .Append<post_service::controllers::UpdatePostHandler>()
                                            .Append<post_service::controllers::DeletePostHandler>()
                                            .Append<post_service::controllers::ToggleLikeHandler>()
                                            .Append<post_service::controllers::GetPostHandler>()
                                            .Append<post_service::controllers::GetUserPostsHandler>()
                                            .Append<post_service::managers::S3Manager>()
                                            .Append<post_service::managers::PostManager>()
                                            .Append<post_service::repositories::PostRepository>();

    return userver::utils::DaemonMain(argc, argv, componentList);
}