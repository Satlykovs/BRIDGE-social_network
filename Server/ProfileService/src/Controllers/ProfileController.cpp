#include "Controllers/ProfileController.hpp"
#include "Managers/S3Manager.hpp"
#include "schemas/Profile.hpp"

#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <fmt/format.h>
#include <userver/s3api/clients/s3api.hpp>
#include <userver/utils/uuid4.hpp>
#include <userver/http/content_type.hpp>
#include <userver/formats/json.hpp>
#include <userver/http/status_code.hpp>

namespace profile_service::controllers
{
    UpdateAvatarHandler::UpdateAvatarHandler(const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context) : HttpHandlerBase(config, context),
        profileManager_(context.FindComponent<profile_service::managers::ProfileManager>()) {}

        std::string UpdateAvatarHandler::HandleRequestThrow(const userver::server::http::HttpRequest& request,
            userver::server::request::RequestContext& context) const
            {
                const auto contentType = userver::http::ContentType(request.GetHeader(userver::http::headers::kContentType));
                if (contentType != "multipart/form-data")
                {
                    request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kBadRequest);
                    return "Expected 'multipart/form-data' content type";
                }

                if (!request.HasFormDataArg("file"))
                {
                    throw userver::server::handlers::ClientError(
                        userver::server::handlers::ExternalBody{"NO IMAGE"});
                }

                const auto& id = std::stoi(request.GetPathArg("id"));
                const auto& image = request.GetFormDataArg("file");

                const auto fileUrl = profileManager_.UpdateAvatar(id, std::string(image.value));
                return userver::formats::json::ToString(userver::formats::json::MakeObject("path", fileUrl));
            }
    
    
    GetInfoHandler::GetInfoHandler(const userver::components::ComponentConfig& config, 
        const userver::components::ComponentContext& context) : HttpHandlerBase(config, context),
        profileManager_(context.FindComponent<profile_service::managers::ProfileManager>()) {}

    std::string GetInfoHandler::HandleRequestThrow(const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext& context) const
        {
            const auto& id = std::stoi(request.GetPathArg("id"));

            const auto info = profileManager_.GetInfo(id);
            if (info == std::nullopt)
            {
                request.GetHttpResponse().SetStatus(userver::server::http::HttpStatus::kNotFound);
                return {};
            }

            return userver::formats::json::ToString(userver::formats::json::ValueBuilder{info.value()}.ExtractValue());
        }
}
