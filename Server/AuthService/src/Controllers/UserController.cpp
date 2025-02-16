#include "Controllers/UserController.hpp"
#include  "schemas/User.hpp"

#include <fmt/format.h>
#include <userver/components/component.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <userver/formats/json.hpp>


namespace auth_service
{
    RegisterHandler::RegisterHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context) : 
    HttpHandlerJsonBase(config, context), userManager_(context.FindComponent<UserManager>()) {}


    userver::formats::json::Value RegisterHandler::HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json,
        userver::server::request::RequestContext& context) const
        {

            
            auto user_data = request_json.As<auth_service::models::UserRegistrationDTO>();

            try
            {
                bool result = userManager_.RegisterUser(user_data);
                if (result)
                {
                    return userver::formats::json::MakeObject("message", fmt::format("User '{}' registered successfully", user_data.email));
                }
                else
                {

                    throw userver::server::handlers::ClientError(
                        userver::server::handlers::ExternalBody{fmt::format("User with email '{}' already exists", user_data.email)});
                }

            }
            catch (const std::invalid_argument& e)
            {
                throw userver::server::handlers::ClientError(userver::server::handlers::ExternalBody{e.what()});
            }
            
            
        }
}