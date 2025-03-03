#include "../../include/Managers/post_manager.hpp"
#include <fmt/format.h>
#include <optional>
#include <stdexcept>

namespace post_service {

PostManager::PostManager(const userver::components::ComponentConfig &config,
                         const userver::components::ComponentContext &context)
    : ComponentBase(config, context),
      PostStorage_(context.FindComponent<PostStorage>()) {
}

bool PostManager::CreatePost(int user_id, const std::string &content) {
  return PostStorage_.CreatePost(user_id, content);
}

bool PostManager::DeletePost(int post_id) {
  std::optional<post_service::models::Post> post =
      PostStorage_.FindPostById(post_id);
  if (!post.has_value()) {
    throw std::runtime_error(fmt::format("Post {} not found", post_id));
  }
  return PostStorage_.DeletePost(post_id);
}

bool PostManager::EditPost(int post_id, const std::string &content) {
  std::optional<post_service::models::Post> post =
      PostStorage_.FindPostById(post_id);
  if (!post.has_value()) {
    throw std::runtime_error(fmt::format("Post {} not found", post_id));
  }
  return PostStorage_.EditPost(post_id, content);
}

std::optional<post_service::models::Post>
PostManager::FindPostById(int post_id) {
  return PostStorage_.FindPostById(post_id);
}

} // namespace post_service
