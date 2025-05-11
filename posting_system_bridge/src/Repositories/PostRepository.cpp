#include "../../include/Repositories/PostRepository.hpp"
#include "../../include/sql/include/sql_queries/sql_queries.hpp"
#include <userver/components/component.hpp>
#include <optional>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/query.hpp>

namespace post_service
{

PostStorage::PostStorage(const userver::components::ComponentConfig &config,
			 const userver::components::ComponentContext &context)
	: userver::components::ComponentBase(config, context)
	, pgCluster_(context.FindComponent<userver::components::Postgres>(
				    "post-db")
			     .GetCluster())
{
}

bool PostStorage::CreatePost(int user_id, const std::string &content)
{
	auto result = pgCluster_->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		sql_queries::sql::kCreatePost, user_id, content);
	return !result.IsEmpty();
}

bool PostStorage::DeletePost(int post_id, int user_id)
{
	auto result = pgCluster_->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		sql_queries::sql::kDeletePost, post_id, user_id);
	return result.RowsAffected() > 0;
}

bool PostStorage::EditPost(int post_id, int user_id, const std::string &edited_content)
{
	auto result = pgCluster_->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		sql_queries::sql::kEditPost, edited_content, post_id, user_id);
	return result.RowsAffected() > 0;
}

std::optional<post_service::models::Post> PostStorage::FindPostById(int post_id)
{
	auto result = pgCluster_->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		sql_queries::sql::kFindPostById, post_id);
	if (!result.IsEmpty()) {
		return result.AsSingleRow<post_service::models::Post>(
			userver::storages::postgres::kRowTag);
	}
	return std::nullopt;
}

bool PostStorage::RemoveReaction(int post_id, int user_id) {
    auto result = pgCluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        sql_queries::sql::kRemoveReaction, post_id, user_id);
    return result.RowsAffected() > 0;
}

bool PostStorage::LikePost(int post_id, int user_id)
{
	auto result = pgCluster_->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		sql_queries::sql::kLikePost, post_id, user_id);
	return result.RowsAffected() > 0;
}

bool PostStorage::DislikePost(int post_id, int user_id)
{
	auto result = pgCluster_->Execute(
		userver::storages::postgres::ClusterHostType::kMaster,
		sql_queries::sql::kDislikePost, post_id, user_id);
	return result.RowsAffected() > 0;
}

} // namespace post_service
