#include "Controllers/AuthController.hpp"
#include "schemas/User.hpp"

#include <fmt/format.h>
#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>

namespace auth_service {
RegisterHandler::RegisterHandler(
    const userver::components::ComponentConfig &config,
    const userver::components::ComponentContext &context)
    : HttpHandlerJsonBase(config, context),
      authManager_(context.FindComponent<AuthManager>()) {}

userver::formats::json::Value RegisterHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest &request,
    const userver::formats::json::Value &request_json,
    userver::server::request::RequestContext &context) const {

    auto user_data =
        request_json.As<auth_service::models::UserDTO>();

    try {
        bool result = authManager_.RegisterUser(user_data);
        if (result) {
            return userver::formats::json::MakeObject(
                "message", fmt::format("User '{}' registered successfully",
                                       user_data.email));
        } else {

            throw userver::server::handlers::ClientError(
                userver::server::handlers::ExternalBody{fmt::format(
                    "User with email '{}' already exists", user_data.email)});
        }

    } catch (const std::exception &e) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{fmt::format("Registration failed: {}", e.what())});
    }
}


LoginHandler::LoginHandler(const userver::components::ComponentConfig& config, 
    const userver::components::ComponentContext& context) : 
    HttpHandlerJsonBase(config, context), authManager_(context.FindComponent<AuthManager>()) {}

userver::formats::json::Value LoginHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest& request, 
    const userver::formats::json::Value& request_json,
    userver::server::request::RequestContext&) const
{
    auto user_data = request_json.As<auth_service::models::UserDTO>();
    try
    {
        std::string token = authManager_.AuthenticateUser(user_data);


        return userver::formats::json::MakeObject("token", token);
    }
    catch (const std::exception&  e)
    {
        throw userver::server::handlers::Unauthorized(
            userver::server::handlers::ExternalBody{fmt::format("Authentication failed: {}", e.what())});
    }
}






void AddAuthController(userver::components::ComponentList& component_list)
{
    component_list
    .Append<auth_service::RegisterHandler>()
    .Append<auth_service::LoginHandler>();
}

} // namespace auth_service