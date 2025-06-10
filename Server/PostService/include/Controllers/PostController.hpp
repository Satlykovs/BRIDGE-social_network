#ifndef POST_CONTROLLER_HPP
#define POST_CONTROLLER_HPP

#include <userver/components/component_fwd.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>

#include "Managers/PostManager.hpp"

namespace post_service::controllers
{
	class CreatePostHandler final
		: public userver::server::handlers::HttpHandlerBase
	{
	   public:
		static constexpr std::string_view kName = "create-post-handler";

		CreatePostHandler(const userver::components::ComponentConfig& config,
						  const userver::components::ComponentContext& context);

		std::string HandleRequestThrow(
			const userver::server::http::HttpRequest& request,
			userver::server::request::RequestContext& context) const;

	   private:
		post_service::managers::PostManager& postManager_;
	};

	class UpdatePostHandler final
		: public userver::server::handlers::HttpHandlerBase
	{
	   public:
		static constexpr std::string_view kName = "update-post-handler";

		UpdatePostHandler(const userver::components::ComponentConfig& config,
						  const userver::components::ComponentContext& context);

		std::string HandleRequestThrow(
			const userver::server::http::HttpRequest& request,
			userver::server::request::RequestContext& context) const;

	   private:
		post_service::managers::PostManager& postManager_;
	};

	class DeletePostHandler final
		: public userver::server::handlers::HttpHandlerJsonBase
	{
	   public:
		static constexpr std::string_view kName = "delete-post-handler";

		DeletePostHandler(const userver::components::ComponentConfig& config,
						  const userver::components::ComponentContext& context);

		userver::formats::json::Value HandleRequestJsonThrow(
			const userver::server::http::HttpRequest& request,
			const userver::formats::json::Value& request_json,
			userver::server::request::RequestContext& context) const;

	   private:
		post_service::managers::PostManager& postManager_;
	};

	class ToggleLikeHandler final
		: public userver::server::handlers::HttpHandlerJsonBase
	{
	   public:
		static constexpr std::string_view kName = "toggle-like-handler";

		ToggleLikeHandler(const userver::components::ComponentConfig& config,
						  const userver::components::ComponentContext& context);

		userver::formats::json::Value HandleRequestJsonThrow(
			const userver::server::http::HttpRequest& request,
			const userver::formats::json::Value& request_json,
			userver::server::request::RequestContext& context) const;

	   private:
		post_service::managers::PostManager& postManager_;
	};

	class GetPostHandler final
		: public userver::server::handlers::HttpHandlerJsonBase
	{
	   public:
		static constexpr std::string_view kName = "get-post-handler";

		GetPostHandler(const userver::components::ComponentConfig& config,
					   const userver::components::ComponentContext& context);

		userver::formats::json::Value HandleRequestJsonThrow(
			const userver::server::http::HttpRequest& request,
			const userver::formats::json::Value& request_json,
			userver::server::request::RequestContext& context) const;

	   private:
		post_service::managers::PostManager& postManager_;
	};

	class GetUserPostsHandler final
		: public userver::server::handlers::HttpHandlerJsonBase
	{
	   public:
		static constexpr std::string_view kName = "get-user-posts-handler";

		GetUserPostsHandler(
			const userver::components::ComponentConfig& config,
			const userver::components::ComponentContext& context);

		userver::formats::json::Value HandleRequestJsonThrow(
			const userver::server::http::HttpRequest& request,
			const userver::formats::json::Value& request_json,
			userver::server::request::RequestContext& context) const;

	   private:
		post_service::managers::PostManager& postManager_;
	};

} // namespace post_service::controllers

#endif