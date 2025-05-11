#include "../../include/Managers/CommentManager.hpp"
#include <fmt/format.h>

namespace post_service {

CommentManager::CommentManager(const ComponentConfig& config,
                               const ComponentContext& context)
    : ComponentBase(config, context),
      storage_(context.FindComponent<CommentStorage>()) {}

bool CommentManager::CreateComment(int post_id, int author_id, const std::string& content) {
  storage_.CreateComment(post_id, author_id, content);
  return true;
}

std::vector<models::Comment> CommentManager::GetComments(int post_id) {
  return storage_.GetCommentsByPost(post_id);
}

}  // namespace post_service