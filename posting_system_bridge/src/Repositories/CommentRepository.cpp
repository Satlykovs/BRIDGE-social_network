#include "../../include/Repositories/CommentRepository.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include <userver/formats/json.hpp>

namespace post_service {

CommentStorage::CommentStorage(const ComponentConfig& config,
                               const ComponentContext& context)
    : ComponentBase(config, context),
      pg_cluster_(context.FindComponent<userver::components::Postgres>("post-db").GetCluster()) {}

models::Comment CommentStorage::CreateComment(int post_id,
                                              int author_id,
                                              const std::string& content) {
  auto result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kMaster,
      "INSERT INTO post_comments(post_id, author_id, content) VALUES($1,$2,$3) RETURNING id, created_at;",
      post_id, author_id, content);
  auto row = result.AsSingleRow();
  models::Comment c;
  c.id = row[0].As<int>();
  c.post_id = post_id;
  c.author_id = author_id;
  c.content = content;
  c.created_at = row[1].As<std::chrono::system_clock::time_point>();
  return c;
}

std::vector<models::Comment> CommentStorage::GetCommentsByPost(int post_id) {
  auto result = pg_cluster_->Execute(
      userver::storages::postgres::ClusterHostType::kSlave,
      "SELECT id, author_id, content, created_at FROM post_comments WHERE post_id=$1 ORDER BY created_at;",
      post_id);
  std::vector<models::Comment> comments;
  for (auto row : result.AsKeyRows()) {
    comments.push_back({
      row[0].As<int>(), post_id,
      row[1].As<int>(), row[2].As<std::string>(),
      row[3].As<std::chrono::system_clock::time_point>()});
  }
  return comments;
}

}  // namespace post_service