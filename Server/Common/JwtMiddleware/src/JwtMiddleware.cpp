#include "JwtMiddleware.hpp"

#include <jwt-cpp/jwt.h>

#include <userver/components/component.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace jwt_middleware
{
	JwtMiddleware::JwtMiddleware(std::string secretKey)
		: secretKey_(std::move(secretKey)){};

	void JwtMiddleware::HandleRequest(
		userver::server::http::HttpRequest& request,
		userver::server::request::RequestContext& context) const
	{
		const auto path = request.GetRequestPath();
		if (IsAuthPath(path))
		{
			Next(request, context);
			return;
		}

		const auto authHeader = request.GetHeader("Authorization");

		if (!authHeader.starts_with("Bearer "))
		{
			throw userver::server::handlers::Unauthorized(
				userver::server::handlers::ExternalBody{
					"Invalid token format"});
		}

		const auto token = authHeader.substr(7);

		if (!VerifyToken(token))
		{
			throw userver::server::handlers::Unauthorized(
				userver::server::handlers::ExternalBody{"Invalid token"});
		}

		try
		{
			const std::string userId = ExtractUserId(token);
			context.SetData("user_id", std::stoi(userId));
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::InternalServerError(
				userver::server::handlers::ExternalBody{e.what()});
		}

		Next(request, context);
	}

	bool JwtMiddleware::IsAuthPath(const std::string& path) const
	{
		return path.starts_with("/auth/");
	}

	bool JwtMiddleware::VerifyToken(const std::string& token) const
	{
		try
		{
			auto decoded = jwt::decode(token);
			auto verifier =
				jwt::verify()
					.allow_algorithm(jwt::algorithm::hs256{secretKey_})
					.with_issuer("auth-service")
					.with_audience("Qt-client")
					.leeway(0);
			verifier.verify(decoded);
			return true;
		}
		catch (const std::exception& e)
		{
			return false;
		}
	}

	std::string JwtMiddleware::ExtractUserId(const std::string& token) const
	{
		try
		{
			auto decoded = jwt::decode(token);

			if (auto subject = decoded.get_subject(); !subject.empty())
				return subject;

			throw std::runtime_error("Token has no subject claim");
		}
		catch (const std::exception& e)
		{
			throw e;
		}
	}

	JwtMiddlewareFactory::JwtMiddlewareFactory(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpMiddlewareFactoryBase(config, context),
		  secretKey_(config["secret-key"].As<std::string>())
	{
	}

	std::unique_ptr<userver::server::middlewares::HttpMiddlewareBase>
	JwtMiddlewareFactory::Create(
		const userver::server::handlers::HttpHandlerBase&,
		userver::yaml_config::YamlConfig) const
	{
		return std::make_unique<JwtMiddleware>(secretKey_);
	}

	userver::yaml_config::Schema JwtMiddlewareFactory::GetStaticConfigSchema()
	{
		return userver::yaml_config::MergeSchemas<
			userver::components::ComponentBase>(
			R"(
            type: object
            description: JwtMiddlewareFactory component
            additionalProperties: false
            properties:
                secret-key:
                    type: string
                    description: Secret jwt key
            )");
	}

} // namespace jwt_middleware