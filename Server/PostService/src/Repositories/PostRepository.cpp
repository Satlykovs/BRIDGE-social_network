#include "Repositories/PostRepository.hpp"

#include <userver/components/component.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/utils/datetime/timepoint_tz.hpp>

#include "schemas/Post.hpp"
#include "sql/include/sql_queries/sql_queries.hpp"

namespace post_service::repositories
{
	PostRepository::PostRepository(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: ComponentBase(config, context),
		  pgCluster_(
			  context.FindComponent<userver::components::Postgres>("post-db")
				  .GetCluster())
	{
	}

	void PostRepository::CreatePost(int userId, const std::string& text,
									const std::string& fileUrl)
	{
		pgCluster_->Execute(
			userver::storages::postgres::ClusterHostType::kMaster,
			sql_queries::sql::kCreatePost, userId, text, fileUrl);
	}

	std::string PostRepository::UpdatePost(int userId, int postId,
										   const std::string& text,
										   const std::string& fileUrl)
	{
		return pgCluster_
			->Execute(userver::storages::postgres::ClusterHostType::kMaster,
					  sql_queries::sql::kUpdatePost, postId, userId, text,
					  fileUrl)
			.AsSingleRow<std::string>();
	}

	std::string PostRepository::DeletePost(int userId, int postId)
	{
		return pgCluster_
			->Execute(userver::storages::postgres::ClusterHostType::kMaster,
					  sql_queries::sql::kDeletePost, postId, userId)
			.AsSingleRow<std::string>();
	}

	bool PostRepository::CheckOwner(int postId, int userId)
	{
		return pgCluster_
			->Execute(userver::storages::postgres::ClusterHostType::kSlave,
					  sql_queries::sql::kCheckOwner, postId, userId)
			.AsSingleRow<bool>();
	}
	bool PostRepository::FindPostById(int postId)
	{
		return pgCluster_
			->Execute(userver::storages::postgres::ClusterHostType::kSlave,
					  sql_queries::sql::kFindPostById, postId)
			.AsSingleRow<bool>();
	}

	bool PostRepository::HasLike(int userId, int postId)
	{
		return pgCluster_
			->Execute(userver::storages::postgres::ClusterHostType::kSlave,
					  sql_queries::sql::kHasLike, postId, userId)
			.AsSingleRow<bool>();
	}

	int PostRepository::AddLike(int userId, int postId)
	{
		return pgCluster_
			->Execute(userver::storages::postgres::ClusterHostType::kMaster,
					  sql_queries::sql::kAddLike, postId, userId)
			.AsSingleRow<int>();
	}

	int PostRepository::RemoveLike(int userId, int postId)
	{
		return pgCluster_
			->Execute(userver::storages::postgres::ClusterHostType::kMaster,
					  sql_queries::sql::kRemoveLike, postId, userId)
			.AsSingleRow<int>();
	}

	std::optional<post_service::models::Post> PostRepository::GetPost(
		int postId)
	{
		auto res = pgCluster_->Execute(
			userver::storages::postgres::ClusterHostType::kSlave,
			sql_queries::sql::kGetPost, postId);

		if (res.IsEmpty()) return std::nullopt;

		return Parser(res[0]);
	}

	std::optional<std::vector<post_service::models::Post>>
	PostRepository::GetPosts(int userId)
	{
		auto res = pgCluster_->Execute(
			userver::storages::postgres::ClusterHostType::kSlave,
			sql_queries::sql::kFindUserPosts, userId);

		if (res.IsEmpty()) return std::nullopt;

		std::vector<post_service::models::Post> posts;
		for (auto row : res)
		{
			posts.push_back(Parser(row));
		}

		return posts;
	}

	post_service::models::Post PostRepository::Parser(
		const userver::storages::postgres::Row& row)
	{
		post_service::models::Post post;

		post.postId = row["post_id"].As<std::optional<int>>();
		post.userId = row["user_id"].As<std::optional<int>>();
		post.text = row["post_text"].As<std::optional<std::string>>();
		post.image_url = row["image_url"].As<std::optional<std::string>>();
		post.likes_count = row["likes_count"].As<std::optional<int>>();
		post.created_at = std::optional<userver::utils::datetime::TimePointTz>(
			row["created_at"]
				.As<userver::storages::postgres::TimePointTz>()
				.GetUnderlying());

		return post;
	}
} // namespace post_service::repositories
