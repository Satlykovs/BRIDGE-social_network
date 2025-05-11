#ifndef COMMENT_MANAGER_HPP
#define COMMENT_MANAGER_HPP

#include "../Repositories/CommentRepository.hpp"
#include <userver/components/component_base.hpp>

namespace post_service {

class CommentManager final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "comment-manager";
  CommentManager(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context);

  bool CreateComment(int post_id, int author_id, const std::string& content);
  std::vector<models::Comment> GetComments(int post_id);

 private:
  CommentStorage& storage_;
};

}  // namespace post_service

#endif  // COMMENT_MANAGER_HPP