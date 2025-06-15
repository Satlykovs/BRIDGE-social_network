#ifndef POST_REPOSITORY_HPP
#define POST_REPOSITORY_HPP

#include <userver/components/component_base.hpp>
#include <userver/components/component_fwd.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/result_set.hpp>

#include "schemas/Post.hpp"

namespace post_service::repositories
{
	class PostRepository final : public userver::components::ComponentBase
	{
	   public:
		static constexpr std::string_view kName = "post-repository";

		PostRepository(const userver::components::ComponentConfig& config,
					   const userver::components::ComponentContext& context);

		post_service::models::Post CreatePost(int userId, const std::string& text,
						const std::string& fileUrl);
		std::pair<std::string, post_service::models::Post> UpdatePost(int userId, int postId, const std::string& text,
							   const std::string& fileUrl);
		std::string DeletePost(int userId, int postId);

		bool CheckOwner(int postId, int userId);
		bool FindPostById(int postId);
		bool HasLike(int userId, int postId);
		int AddLike(int userId, int postId);
		int RemoveLike(int userId, int postId);

		std::optional<post_service::models::Post> GetPost(int postId, int userId);
		std::optional<std::vector<post_service::models::Post>> GetPosts(
			int userId, int targetUserId);

	   private:
		userver::storages::postgres::ClusterPtr pgCluster_;

		post_service::models::Post Parser(
			const userver::storages::postgres::Row& row);
	};
} // namespace post_service::repositories

#endif