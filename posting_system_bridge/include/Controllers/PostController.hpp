ъ#ifndef POST_CONTROLLER_HPP
#define POST_CONTROLLER_HPP

#include <string_view>

#include <userver/components/component_config.hpp>    
#include <userver/components/component_context.hpp>   
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/server/http/http_request.hpp>  
#include <userver/server/http/http_response.hpp>   
#include <userver/formats/json.hpp>
#include <userver/utils/exception.hpp>

#include "../Managers/PostManager.hpp"

namespace post_service {

class PostHandler final
    : public userver::server::handlers::HttpHandlerJsonBase {
public:
    static constexpr std::string_view kName = "post-controller";

    PostHandler(const userver::components::ComponentConfig &config,
                const userver::components::ComponentContext &context);

    userver::formats::json::Value HandleRequestJsonThrow(
        const userver::server::http::HttpRequest &request,
        const userver::formats::json::Value &request_json,
        userver::server::request::RequestContext &context) const override;

private:
    userver::formats::json::Value
    HandleCreatePost(const userver::formats::json::Value &request_json,
                     userver::server::http::HttpResponse &response) const;

    userver::formats::json::Value
    HandleDeletePost(const userver::server::http::HttpRequest &request,
                     userver::server::http::HttpResponse &response) const;

    userver::formats::json::Value
    HandleEditPost(const userver::formats::json::Value &request_json,
                   const userver::server::http::HttpRequest &request,
                   userver::server::http::HttpResponse &response) const;

    userver::formats::json::Value
    HandleFindPostById(const userver::server::http::HttpRequest &request,
                       userver::server::http::HttpResponse &response) const;

    userver::formats::json::Value
    HandleRemoveReaction(const userver::server::http::HttpRequest &request,
                         const userver::formats::json::Value &request_json,
                         userver::server::http::HttpResponse &response) const;

    userver::formats::json::Value
    HandleLikePost(const userver::server::http::HttpRequest &request,
                   const userver::formats::json::Value &request_json,
                   userver::server::http::HttpResponse &response) const;

    userver::formats::json::Value
    HandleDislikePost(const userver::server::http::HttpRequest &request,
                      const userver::formats::json::Value &request_json,
                      userver::server::http::HttpResponse &response) const;

    PostManager &post_manager_;
};

}  // namespace post_service

#endif  // POST_CONTROLLER_HPP
