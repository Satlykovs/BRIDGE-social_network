#include "Controllers/AuthController.hpp"

#include <fmt/format.h>

#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>

#include "schemas/User.hpp"

namespace auth_service::controllers
{
	RegisterHandler::RegisterHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  authManager_(
			  context.FindComponent<auth_service::managers::AuthManager>())
	{
	}

	userver::formats::json::Value RegisterHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& requestJson,
		userver::server::request::RequestContext& context) const
	{
		auto userData = requestJson.As<auth_service::models::UserDTO>();

		try
		{
			authManager_.RegisterUser(userData);

			return userver::formats::json::MakeObject(
				"message", fmt::format("User '{}' registered successfully",
									   userData.email));
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					fmt::format("User with email '{}' already exists or "
								"something else went wrong",
								userData.email)});
		}
	}

	LoginHandler::LoginHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  authManager_(
			  context.FindComponent<auth_service::managers::AuthManager>())
	{
	}

	userver::formats::json::Value LoginHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& requestJson,
		userver::server::request::RequestContext& context) const
	{
		auto userData = requestJson.As<auth_service::models::UserDTO>();
		try
		{
			auto [accessToken, refreshToken] =
				authManager_.AuthenticateUser(userData);

			auto& response = request.GetHttpResponse();
			response.SetHeader(std::string_view("Authorization"),
							   "Bearer " + accessToken.first);
			response.SetHeader(std::string_view("X-Refresh-Token"),
							   refreshToken.first);
			response.SetStatus(userver::server::http::HttpStatus::kOk);

			userver::formats::json::ValueBuilder valueBuilder;
			valueBuilder["acсess-token-exp"] = accessToken.second;
			valueBuilder["refresh-token-exp"] = refreshToken.second;

			return valueBuilder.ExtractValue();
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::Unauthorized(
				userver::server::handlers::ExternalBody{
					fmt::format("Authentication failed: {}", e.what())});
		}
	}

	RefreshHandler::RefreshHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  jwtManager_(
			  context.FindComponent<auth_service::managers::JwtManager>())
	{
	}

	userver::formats::json::Value RefreshHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& requestJson,
		userver::server::request::RequestContext& context) const
	{
		std::string refreshToken = request.GetHeader("X-Refresh-Token");

		if (refreshToken == "")
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{"TOKEN_NOT_FOUND"});
		}

		try
		{
			auto [newAccessToken, newRefreshToken] =
				jwtManager_.RefreshTokens(refreshToken);

			auto& response = request.GetHttpResponse();
			response.SetHeader(std::string_view("Authorization"),
							   "Bearer " + newAccessToken.first);
			response.SetHeader(std::string_view("X-Refresh-Token"),
							   newRefreshToken.first);
			response.SetStatus(userver::server::http::HttpStatus::kOk);

			userver::formats::json::ValueBuilder valueBuilder;
			valueBuilder["access-token-exp"] = newAccessToken.second;
			valueBuilder["refresh-token-exp"] = newRefreshToken.second;

			return valueBuilder.ExtractValue();
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					fmt::format("ERROR: {}", e.what())});
		}
	}

	void AddAuthController(userver::components::ComponentList& componentList)
	{
		componentList.Append<RegisterHandler>()
			.Append<LoginHandler>()
			.Append<RefreshHandler>();
	}

} // namespace auth_service::controllers