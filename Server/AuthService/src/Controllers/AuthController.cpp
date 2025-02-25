#include "Controllers/AuthController.hpp"

#include <fmt/format.h>

#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>

#include "schemas/User.hpp"

namespace auth_service
{
	RegisterHandler::RegisterHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  authManager_(context.FindComponent<AuthManager>())
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
			bool result = authManager_.RegisterUser(userData);
			if (result)
			{
				return userver::formats::json::MakeObject(
					"message", fmt::format("User '{}' registered successfully",
										   userData.email));
			}
			else
			{
				throw userver::server::handlers::ClientError(
					userver::server::handlers::ExternalBody{
						fmt::format("User with email '{}' already exists",
									userData.email)});
			}
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{
					fmt::format("Registration failed: {}", e.what())});
		}
	}

	LoginHandler::LoginHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  authManager_(context.FindComponent<AuthManager>())
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
			std::pair<std::string, std::string> tokens = authManager_.AuthenticateUser(userData);

			userver::formats::json::ValueBuilder valueBuilder;
			valueBuilder["access-token"] = tokens.first;
			valueBuilder["refresh-token"] = tokens.second;
			return valueBuilder.ExtractValue();
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::Unauthorized(
				userver::server::handlers::ExternalBody{
					fmt::format("Authentication failed: {}", e.what())});
		}
	}

	

	RefreshHandler::RefreshHandler(const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  jwtManager_(context.FindComponent<JwtManager>())
	{
	}


	userver::formats::json::Value RefreshHandler::HandleRequestJsonThrow(const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& requestJson,
		userver::server::request::RequestContext& context) const
	{
		std::string refreshToken = request.GetHeader("X-Refresh-Token");


		try
		{
			auto [newAccessToken, newRefreshToken] = jwtManager_.RefreshTokens(refreshToken);

			auto& response = request.GetHttpResponse();
			response.SetHeader(std::string_view("Authorization"), "Bearer " + newAccessToken);
			response.SetHeader(std::string_view("X-Refresh-Token"), newRefreshToken);
			response.SetStatus(userver::server::http::HttpStatus::kOk);

			return userver::formats::json::MakeObject(
				"message", "Token refreshed successfully");
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{fmt::format("ERROR: {}", e.what())});
		}
	}

	void AddAuthController(userver::components::ComponentList& componentList)
	{
		componentList.Append<RegisterHandler>()
			.Append<LoginHandler>()
			.Append<RefreshHandler>();
	}

} // namespace auth_service