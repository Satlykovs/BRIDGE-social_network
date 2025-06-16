#include "Controllers/ProfileController.hpp"

#include <fmt/format.h>

#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/formats/json.hpp>
#include <userver/http/content_type.hpp>
#include <userver/http/status_code.hpp>
#include <userver/s3api/clients/s3api.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/http/http_method.hpp>
#include <userver/utils/uuid4.hpp>

#include "Managers/S3Manager.hpp"
#include "schemas/Profile.hpp"

namespace profile_service::controllers
{
	UpdateAvatarHandler::UpdateAvatarHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerBase(config, context),
		  profileManager_(
			  context
				  .FindComponent<profile_service::managers::ProfileManager>())
	{
	}

	std::string UpdateAvatarHandler::HandleRequestThrow(
		const userver::server::http::HttpRequest& request,
		userver::server::request::RequestContext& context) const
	{
		const auto contentType = userver::http::ContentType(
			request.GetHeader(userver::http::headers::kContentType));
		if (contentType != "multipart/form-data")
		{
			request.GetHttpResponse().SetStatus(
				userver::server::http::HttpStatus::kBadRequest);
			return "Expected 'multipart/form-data' content type";
		}

		if (!request.HasFormDataArg("file"))
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{"NO IMAGE"});
		}

		const int& id = context.GetData<int>("user_id");
		const auto& image = request.GetFormDataArg("file");

		const auto fileUrl =
			profileManager_.UpdateAvatar(id, std::string(image.value));
		return userver::formats::json::ToString(
			userver::formats::json::MakeObject("path", fileUrl));
	}

	InfoHandler::InfoHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  profileManager_(
			  context
				  .FindComponent<profile_service::managers::ProfileManager>())
	{
	}

	userver::formats::json::Value InfoHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& requestJson,
		userver::server::request::RequestContext& context) const
	{
		const int& id = context.GetData<int>("user_id");

		if (request.GetMethod() == userver::server::http::HttpMethod::kGet)
		{
			const auto info = profileManager_.GetInfo(id);
			if (info == std::nullopt)
			{
				request.GetHttpResponse().SetStatus(
					userver::server::http::HttpStatus::kNotFound);
				return {};
			}

			return userver::formats::json::ValueBuilder(info.value())
				.ExtractValue();
		}

		else if (request.GetMethod() ==
				 userver::server::http::HttpMethod::kPost)
		{
			auto dto =
				requestJson.As<profile_service::models::ProfileUpdateDTO>();

			auto newInfo = profileManager_.UpdateInfo(id, dto);

			return userver::formats::json::ValueBuilder(newInfo).ExtractValue();
		}
		else
		{
			auto& httpResponse = request.GetHttpResponse();
			httpResponse.SetStatus(
				userver::server::http::HttpStatus::kMethodNotAllowed);
			return {};
		}
	}

	OtherUserInfoHandler::OtherUserInfoHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  profileManager_(
			  context
				  .FindComponent<profile_service::managers::ProfileManager>())
	{
	}

	userver::formats::json::Value OtherUserInfoHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& requestJson,
		userver::server::request::RequestContext& context) const
	{
		return userver::formats::json::ValueBuilder(
				   profileManager_.GetInfo(std::stoi(request.GetPathArg("id")))
					   .value())
			.ExtractValue();
	}
} // namespace profile_service::controllers
