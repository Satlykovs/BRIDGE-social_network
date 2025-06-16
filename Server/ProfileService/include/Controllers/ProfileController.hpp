#ifndef PROFILE_CONTROLLER_HPP
#define PROFILE_CONTROLLER_HPP

#include <userver/components/component_fwd.hpp>
#include <userver/formats/json/value.hpp>
#include <userver/http/common_headers.hpp>
#include <userver/s3api/clients/fwd.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>

#include "Managers/ProfileManager.hpp"

namespace profile_service::controllers
{
	class UpdateAvatarHandler final
		: public userver::server::handlers::HttpHandlerBase
	{
	   public:
		static constexpr std::string_view kName = "update-avatar-handler";

		UpdateAvatarHandler(
			const userver::components::ComponentConfig& config,
			const userver::components::ComponentContext& context);

		std::string HandleRequestThrow(
			const userver::server::http::HttpRequest& request,
			userver::server::request::RequestContext& context) const override;

	   private:
		profile_service::managers::ProfileManager& profileManager_;
	};

	class InfoHandler final
		: public userver::server::handlers::HttpHandlerJsonBase
	{
	   public:
		static constexpr std::string_view kName = "info-handler";

		InfoHandler(const userver::components::ComponentConfig& config,
					const userver::components::ComponentContext& context);

		userver::formats::json::Value HandleRequestJsonThrow(
			const userver::server::http::HttpRequest& request,
			const userver::formats::json::Value& requestJson,
			userver::server::request::RequestContext& context) const override;

	   private:
		profile_service::managers::ProfileManager& profileManager_;
	};

} // namespace profile_service::controllers

#endif