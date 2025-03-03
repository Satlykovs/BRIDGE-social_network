#include "../../include/Models/post_storage.hpp"
#include <optional>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/query.hpp>
#include "../../include/sql/sql_queries.hpp"

namespace post_service {

PostStorage::PostStorage(const userver::components::ComponentConfig &config,
                         const userver::components::ComponentContext &context)
    : ComponentBase(config, context),
      pgCluster_(context.FindComponent<userver::components::Postgres>("post-db")
                     .GetCluster()) {
}

bool PostStorage::CreatePost(int user_id, const std::string &content) {
  pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                      sql_queries::sql::kCreatePost, user_id, content);
  return true;
}

bool PostStorage::DeletePost(int post_id) {
  pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                      sql_queries::sql::kDeletePost, post_id);
  return true;
}

bool PostStorage::EditPost(int post_id, const std::string &edited_content) {
  pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                      sql_queries::sql::kEditPost, edited_content, post_id);
  return true;
}

std::optional<post_service::models::Post>
PostStorage::FindPostById(int post_id) {
  auto result =
      pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,
                          sql_queries::sql::kFindPostById, post_id);
  if (!result.IsEmpty()) {
    return result.AsSingleRow<post_service::models::Post>(
        userver::storages::postgres::kRowTag);
  }
  return std::nullopt;
}

} // namespace post_service
