#include "../../include/Managers/PostManager.hpp"
#include <userver/components/component.hpp>
#include <fmt/format.h>
#include <optional>

namespace post_service {

PostManager::PostManager(const userver::components::ComponentConfig &config,
                         const userver::components::ComponentContext &context)
    : ComponentBase(config, context),
      PostStorage_(context.FindComponent<PostStorage>()) {
}

bool PostManager::CreatePost(int user_id, const std::string &content) {
  return PostStorage_.CreatePost(user_id, content);
}

bool PostManager::DeletePost(int post_id, int user_id) {
    return PostStorage_.DeletePost(post_id, user_id);
}

bool PostManager::EditPost(int post_id, int user_id, const std::string &content) {
    return PostStorage_.EditPost(post_id, user_id, content);
}

std::optional<post_service::models::Post>
PostManager::FindPostById(int post_id) {
    return PostStorage_.FindPostById(post_id);
}

bool PostManager::RemoveReaction(int post_id, int user_id) {
    return PostStorage_.RemoveReaction(post_id, user_id);
}

bool PostManager::LikePost(int post_id, int user_id) {
    return PostStorage_.LikePost(post_id, user_id);
}

bool PostManager::DislikePost(int post_id, int user_id) {
    return PostStorage_.DislikePost(post_id, user_id);
}

} // namespace post_service
