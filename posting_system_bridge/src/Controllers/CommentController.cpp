#include "../../include/Controllers/CommentController.hpp"
#include "../../include/schemas/CommentRequests.hpp"
#include <userver/formats/json.hpp>
#include <userver/logging/log.hpp>

namespace post_service {

CommentHandler::CommentHandler(const ComponentConfig& config,
                               const ComponentContext& context)
    : HttpHandlerJsonBase(config, context) {}

userver::formats::json::Value CommentHandler::HandleRequestThrow(
    const server::http::HttpRequest& request,
    const userver::formats::json::Value& request_json,
    server::http::HttpResponse& response) const {
  const auto method = request.GetMethod();
  auto& manager = request.FindComponent<CommentManager>();

  if (method == server::http::HttpMethod::kPost) {
    models::CreateCommentDTO dto{request_json};
    manager.CreateComment(dto.post_ID, dto.author_ID, dto.content);
    response.SetStatus(server::http::HttpStatus::kCreated);
    return userver::formats::json::MakeObject("message", "Comment created");
  }

  if (method == server::http::HttpMethod::kGet) {
    int post_id = std::stoi(request.GetPathArg("post_id"));
    auto comments = manager.GetComments(post_id);
    return userver::formats::json::ToValue(comments);
  }

  throw std::runtime_error("Unsupported method");
}

}  // namespace post_service