#ifndef COMMENT_REPOSITORY_HPP
#define COMMENT_REPOSITORY_HPP

#include <string>
#include <vector>
#include <chrono>
#include <userver/components/component_base.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace post_service::models {
struct Comment {
  int id;
  int post_id;
  int author_id;
  std::string content;
  std::chrono::system_clock::time_point created_at;
};
}  // namespace post_service::models

namespace post_service {

class CommentStorage final : public userver::components::ComponentBase {
 public:
  static constexpr std::string_view kName = "comment-storage";
  CommentStorage(const userver::components::ComponentConfig& config,
                 const userver::components::ComponentContext& context);

  models::Comment CreateComment(int post_id,
                                 int author_id,
                                 const std::string& content);

  std::vector<models::Comment> GetCommentsByPost(int post_id);

 private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

}  // namespace post_service

#endif  // COMMENT_REPOSITORY_HPP