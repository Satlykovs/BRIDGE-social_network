#include "../../include/Controllers/PostController.hpp"
#include "../../include/schemas/PostRequests.hpp"

#include <fmt/format.h>
#include <userver/formats/json/value_builder.hpp>
#include <userver/server/request/request_context.hpp>
#include <userver/logging/log.hpp>
#include <userver/formats/json.hpp>
#include <userver/formats/parse/common_containers.hpp>

namespace post_service
{

PostHandler::PostHandler(const userver::components::ComponentConfig &config,
			 const userver::components::ComponentContext &context)
	: userver::server::handlers::HttpHandlerJsonBase(config, context)
	, post_manager_(context.FindComponent<post_service::PostManager>(
		  PostManager::kName))
{
}

userver::formats::json::Value PostHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest &request,
    const userver::formats::json::Value &request_json,
    userver::server::request::RequestContext &) const
{
    try {
        if (request.GetMethod() == userver::server::http::HttpMethod::kPost) {
            if (request.GetRequestPath().ends_with("/like")) {
                return HandleLikePost(request, request_json, request.GetHttpResponse());
            } else if (request.GetRequestPath().ends_with("/dislike")) {
                return HandleDislikePost(request, request_json, request.GetHttpResponse());
            } else if (request.GetRequestPath().ends_with("/remove-reaction")) {
                return HandleRemoveReaction(request, request_json, request.GetHttpResponse());
            }
            return HandleCreatePost(request_json, request.GetHttpResponse());
        } else if (request.GetMethod() == userver::server::http::HttpMethod::kDelete) {
            return HandleDeletePost(request, request.GetHttpResponse());
        } else if (request.GetMethod() == userver::server::http::HttpMethod::kPut) {
            return HandleEditPost(request_json, request, request.GetHttpResponse());
        } else {
            throw userver::server::handlers::ClientError(
                userver::server::handlers::ExternalBody{
                    "Unsupported HTTP method" });
        }
    } catch (const std::exception &e) {
        throw userver::server::handlers::ClientError(
            userver::server::handlers::ExternalBody{
                fmt::format("Error: {}", e.what()) });
    }
}

userver::formats::json::Value PostHandler::HandleCreatePost(
	const userver::formats::json::Value &request_json,
	userver::server::http::HttpResponse &response) const
{
	auto post_data =
		request_json.As<post_service::models::CreateEditPostDTO>();
	bool result = post_manager_.CreatePost(post_data.current_user_ID,
					       post_data.text);
	if (!result) {
		throw userver::server::handlers::InternalServerError(
			userver::server::handlers::ExternalBody{
				"Post creation failed" });
	}
	response.SetStatus(userver::server::http::HttpStatus::kCreated);
	return userver::formats::json::MakeObject(
		"message",
		fmt::format("Post '{}' created successfully", post_data.text));
}

userver::formats::json::Value PostHandler::HandleDeletePost(
    const userver::server::http::HttpRequest &request,
    userver::server::http::HttpResponse &response) const {
  const int post_id = std::stoi(request.GetPathArg("id"));
  const int user_id = std::stoi(request.GetArg("user_id"));

  bool result = post_manager_.DeletePost(post_id, user_id);
  if (result) {
    response.SetStatus(userver::server::http::HttpStatus::kOk);
    return userver::formats::json::MakeObject(
        "message", fmt::format("Post {} deleted", post_id));
  }
  throw userver::server::handlers::ClientError(
      userver::server::handlers::ExternalBody{"Delete failed"});
}

userver::formats::json::Value PostHandler::HandleEditPost(
    const userver::formats::json::Value &request_json,
    const userver::server::http::HttpRequest &request,
    userver::server::http::HttpResponse &response) const {
  auto post_data = request_json.As<models::CreateEditPostDTO>();
  const int post_id = std::stoi(request.GetPathArg("id"));

  bool result = post_manager_.EditPost(post_id, post_data.current_user_ID, post_data.text);
  if (result) {
    response.SetStatus(userver::server::http::HttpStatus::kOk);
    return userver::formats::json::MakeObject(
        "message", fmt::format("Post {} updated", post_id));
  }
  throw userver::server::handlers::ClientError(
      userver::server::handlers::ExternalBody{"Update failed"});
}

userver::formats::json::Value PostHandler::HandleFindPostById(
	const userver::server::http::HttpRequest &request,
	userver::server::http::HttpResponse &response) const
{
	const int post_id = std::stoi(request.GetPathArg("id"));

	auto post_opt = post_manager_.FindPostById(post_id);
	if (!post_opt.has_value()) {
		throw userver::server::handlers::ClientError(
			userver::server::handlers::ExternalBody{ fmt::format(
				"Post with ID {} not found", post_id) });
	}

	const post_service::models::Post &post = *post_opt;
	response.SetStatus(userver::server::http::HttpStatus::kOk);
	return userver::formats::json::MakeObject("post_id", post.post_id,
						  "user_id", post.user_id,
						  "content", post.content,
						  "created_at", post.created_at,
						  "likes", post.likes,
                          "dislikes", post.dislikes
	);
}

userver::formats::json::Value PostHandler::HandleRemoveReaction(
    const userver::server::http::HttpRequest &request,
    const userver::formats::json::Value &request_json,
    userver::server::http::HttpResponse &response) const {

    const int post_id = std::stoi(request.GetPathArg("id"));
    const int user_id = request_json["user_id"].As<int>();

    bool result = post_manager_.RemoveReaction(post_id, user_id);
    if (!result) {
        response.SetStatus(userver::server::http::HttpStatus::kNotFound);
        return userver::formats::json::MakeObject(
            "message", fmt::format("No reaction found for user {} on post {}", user_id, post_id));
    }

    response.SetStatus(userver::server::http::HttpStatus::kOk);
    return userver::formats::json::MakeObject(
        "message", fmt::format("Reaction removed from post {}", post_id));
}

userver::formats::json::Value PostHandler::HandleLikePost(
    const userver::server::http::HttpRequest &request,
    const userver::formats::json::Value &request_json,
    userver::server::http::HttpResponse &response) const {
  const int post_id = std::stoi(request.GetPathArg("id"));
  const int user_id = request_json["user_id"].As<int>();

  post_manager_.LikePost(post_id, user_id);
  response.SetStatus(userver::server::http::HttpStatus::kOk);
  return userver::formats::json::MakeObject(
      "message", fmt::format("Post {} liked", post_id));
}

userver::formats::json::Value PostHandler::HandleDislikePost(
    const userver::server::http::HttpRequest &request,
    const userver::formats::json::Value &request_json,
    userver::server::http::HttpResponse &response) const {
  const int post_id = std::stoi(request.GetPathArg("id"));
  const int user_id = request_json["user_id"].As<int>();

  post_manager_.DislikePost(post_id, user_id);
  response.SetStatus(userver::server::http::HttpStatus::kOk);
  return userver::formats::json::MakeObject(
      "message", fmt::format("Post {} disliked", post_id));
}


} // namespace post_service
