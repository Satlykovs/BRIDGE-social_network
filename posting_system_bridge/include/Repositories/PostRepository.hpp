#ifndef POST_STORAGE_HPP
#define POST_STORAGE_HPP

#include <string>
#include <userver/components/component_base.hpp>
#include <userver/components/component_fwd.hpp>
#include <userver/storages/postgres/cluster.hpp>

namespace post_service::models
{
struct Post {
	int post_id;
	int user_id;
	std::string content;
	std::chrono::system_clock::time_point created_at;
	int likes;
	int dislikes;
};

} // namespace post_service::models

namespace post_service
{
class PostStorage final : public userver::components::ComponentBase {
    public:
	static constexpr std::string_view kName = "post-storage";
	PostStorage(const userver::components::ComponentConfig &config,
		    const userver::components::ComponentContext &context);

	bool CreatePost(int user_id, const std::string &content);
	bool DeletePost(int post_id, int user_id);
	bool EditPost(int post_id, int user_id,
		      const std::string &edited_content);
	std::optional<post_service::models::Post> FindPostById(int post_id);
	bool RemoveReaction(int post_id, int user_id);
	bool LikePost(int post_id, int user_id);
	bool DislikePost(int post_id, int user_id);

    private:
	userver::storages::postgres::ClusterPtr pgCluster_;
};
} // namespace post_service

#endif
