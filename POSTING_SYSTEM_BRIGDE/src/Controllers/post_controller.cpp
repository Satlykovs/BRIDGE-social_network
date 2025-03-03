#include "../../include/Controllers/post_controller.hpp"

#include <userver/server/request/request_context.hpp>
#include <userver/utils/logger.hpp>

namespace post_service {

PostHandler::PostHandler(const userver::components::ComponentConfig &config,
                         const userver::components::ComponentContext &context)
    : userver::server::handlers::HttpHandlerJsonBase(config, context),
      post_manager_(context.FindComponent<post_service::PostManager>(
          PostManager::kName)) {}

userver::formats::json::Value PostHandler::HandleRequestJsonThrow(
    const userver::server::http::HttpRequest &request,
    const userver::formats::json::Value &request_json,
    userver::server::request::RequestContext &) const {
  try {
    if (request.GetMethod() == userver::server::http::HttpMethod::kPost) {
      return HandleCreatePost(request_json, *request.GetResponse());
    } else if (request.GetMethod() ==
               userver::server::http::HttpMethod::kDelete) {
      return HandleDeletePost(request, *request.GetResponse());
    } else if (request.GetMethod() == userver::server::http::HttpMethod::kPut) {
      return HandleEditPost(request_json, request, *request.GetResponse());
    } else if (request.GetMethod() == userver::server::http::HttpMethod::kGet) {
      return HandleFindPostById(request, *request.GetResponse());
    } else {
      throw userver::server::handlers::ClientError(
          userver::server::handlers::ExternalBody{"Unsupported HTTP method"});
    }
  } catch (const std::exception &e) {
    // Wrap any exception into a ClientError (HTTP 400) with details
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            fmt::format("Error: {}", e.what())});
  }
}

userver::formats::json::Value
PostHandler::HandleCreatePost(const userver::formats::json::Value &request_json,
                              userver::server::http::HttpResponse &response) {
  auto post_data = request_json.As<post_service::models::CreatePostDTO>();
  bool result = post_manager_.CreatePost(post_data.user_id, post_data.content);
  if (!result) {

    throw userver::server::handlers::InternalServerError(
        userver::server::handlers::ExternalBody{"Post creation failed"});
  }
  response.SetStatus(userver::server::http::HttpStatus::kCreated);
  response.SetBody(fmt::format(
      R"({{"message": "Post '{}' created successfully"}})", post_data.content));
  return userver::formats::json::Value(); // JSON response already set in body
}

userver::formats::json::Value
PostHandler::HandleDeletePost(const userver::server::http::HttpRequest &request,
                              userver::server::http::HttpResponse &response) {

  const std::string post_id_str = request.GetArg("id");
  if (post_id_str.empty()) {
    throw std::invalid_argument("Missing 'id' query parameter");
  }
  int post_id = std::stoi(post_id_str);
  bool result = post_manager_.DeletePost(post_id);
  if (result) {
    response.SetStatus(userver::server::http::HttpStatus::kOk);
    response.SetBody(fmt::format(
        R"({{"message": "Post with ID {} deleted successfully"}})", post_id));
  }
  return userver::formats::json::Value();
}

userver::formats::json::Value
PostHandler::HandleEditPost(const userver::formats::json::Value &request_json,
                            const userver::server::http::HttpRequest &request,
                            userver::server::http::HttpResponse &response) {

  auto post_data = request_json.As<post_service::models::CreatePostDTO>();
  const std::string post_id_str = request.GetArg("id");
  if (post_id_str.empty()) {
    throw std::invalid_argument("Missing 'id' query parameter");
  }
  int post_id = std::stoi(post_id_str);
  bool result = post_manager_.EditPost(post_id, post_data.content);
  if (result) {
    response.SetStatus(userver::server::http::HttpStatus::kOk);
    response.SetBody(
        fmt::format(R"({{"message": "Post '{}' edited successfully"}})",
                    post_data.content));
  }
  return userver::formats::json::Value();
}

userver::formats::json::Value PostHandler::HandleFindPostById(
    const userver::server::http::HttpRequest &request,
    userver::server::http::HttpResponse &response) {
  const std::string post_id_str = request.GetArg("id");
  if (post_id_str.empty()) {
    throw std::invalid_argument("Missing 'id' query parameter");
  }
  int post_id = std::stoi(post_id_str);

  auto post_opt = post_manager_.FindPostById(post_id);
  if (!post_opt.has_value()) {
    throw userver::server::handlers::ClientError(
        userver::server::handlers::ExternalBody{
            fmt::format("Post with ID {} not found", post_id)});
  }

  const post_service::models::Post &post = *post_opt;
  response.SetStatus(userver::server::http::HttpStatus::kOk);
  response.SetBody(
      fmt::format(R"({{"post_id": {}, "user_id": {}, "content": "{}"}})",
                  post.post_id, post.user_id, post.content));
  return userver::formats::json::Value();
}

} // namespace post_service
