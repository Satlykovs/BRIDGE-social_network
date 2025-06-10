#include "Controllers/PostController.hpp"

#include <userver/components/component.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/http/common_headers.hpp>
#include <userver/http/content_type.hpp>
#include <userver/http/status_code.hpp>
#include <userver/storages/postgres/exceptions.hpp>

namespace post_service::controllers
{
	CreatePostHandler::CreatePostHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerBase(config, context),
		  postManager_(
			  context.FindComponent<post_service::managers::PostManager>())
	{
	}

	UpdatePostHandler::UpdatePostHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerBase(config, context),
		  postManager_(
			  context.FindComponent<post_service::managers::PostManager>())
	{
	}

	DeletePostHandler::DeletePostHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  postManager_(
			  context.FindComponent<post_service::managers::PostManager>())
	{
	}

	ToggleLikeHandler::ToggleLikeHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  postManager_(
			  context.FindComponent<post_service::managers::PostManager>())
	{
	}

	GetPostHandler::GetPostHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  postManager_(
			  context.FindComponent<post_service::managers::PostManager>())
	{
	}

	GetUserPostsHandler::GetUserPostsHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  postManager_(
			  context.FindComponent<post_service::managers::PostManager>())
	{
	}

	std::string CreatePostHandler::HandleRequestThrow(
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

		if (!request.HasFormDataArg("text"))
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{"NO TEXT IN POST"});
		}

		const int userId = context.GetData<int>("user_id");
		std::string text = std::string(request.GetFormDataArg("text").value);
		std::optional<std::string> image = std::nullopt;

		if (request.HasFormDataArg("image"))
		{
			image = request.GetFormDataArg("image").value;
		}

		try
		{
			postManager_.CreatePost(userId, text, image);
			return {};
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::InternalServerError();
		}
	}

	std::string UpdatePostHandler::HandleRequestThrow(
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

		if (!request.HasFormDataArg("text"))
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{"NO TEXT IN POST"});
		}

		const int userId = context.GetData<int>("user_id");
		const int postId = std::stoi(request.GetPathArg("post_id"));
		std::string text = std::string(request.GetFormDataArg("text").value);
		std::optional<std::string> image = std::nullopt;

		if (request.HasFormDataArg("image"))
		{
			image = request.GetFormDataArg("image").value;
		}

		try
		{
			postManager_.UpdatePost(userId, postId, text, image);
			return {};
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::InternalServerError();
		}
	}

	userver::formats::json::Value DeletePostHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& request_json,
		userver::server::request::RequestContext& context) const
	{
		const int userId = context.GetData<int>("user_id");
		const int postId = std::stoi(request.GetPathArg("post_id"));

		postManager_.DeletePost(userId, postId);
		return {};
	}

	userver::formats::json::Value ToggleLikeHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& request_json,
		userver::server::request::RequestContext& context) const
	{
		const int userId = context.GetData<int>("user_id");
		const int postId = std::stoi(request.GetPathArg("post_id"));

		auto res = postManager_.ToggleLike(userId, postId);
		userver::formats::json::ValueBuilder builder;

		builder["count"] = res.first;
		builder["i_liked"] = res.second;

		return builder.ExtractValue();
	}

	userver::formats::json::Value GetPostHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& request_json,
		userver::server::request::RequestContext& context) const
	{
		const int postId = std::stoi(request.GetPathArg("post_id"));

		auto post = postManager_.GetPost(postId);
		return userver::formats::json::ValueBuilder(post).ExtractValue();
	}

	userver::formats::json::Value GetUserPostsHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& request_json,
		userver::server::request::RequestContext& context) const
	{
		const int userId = context.GetData<int>("user_id");
		const int targetUserId = std::stoi(request.GetPathArg("user_id"));

		auto posts = postManager_.GetUserPosts(targetUserId);

		return userver::formats::json::ValueBuilder(posts).ExtractValue();
	}
} // namespace post_service::controllers