#ifndef PROFILE_MANAGER_HPP
#define PROFILE_MANAGER_HPP

#include <userver/components/component_base.hpp>
#include <userver/s3api/clients/fwd.hpp>

#include "Repositories/PostRepository.hpp"
#include "schemas/Post.hpp"

namespace post_service::managers
{
	class PostManager final : public userver::components::ComponentBase
	{
	   public:
		static constexpr std::string_view kName = "post-manager";

		PostManager(const userver::components::ComponentConfig& config,
					const userver::components::ComponentContext& context);

		post_service::models::Post CreatePost(int userId, const std::string& text,
						std::optional<std::string>& image);
		post_service::models::Post UpdatePost(int userId, int postId, const std::string& text,
						std::optional<std::string>& image);
		void DeletePost(int userId, int postId);
		std::pair<int, bool> ToggleLike(int userId, int postId);
		post_service::models::Post GetPost(int postId, int userId);
		std::vector<post_service::models::Post> GetUserPosts(int userId, int targetUserId);

	   private:
		userver::s3api::ClientPtr s3Client_;
		post_service::repositories::PostRepository& postRepository_;
		static constexpr std::string_view kPngMagicBytes = "\x89PNG\r\n\x1a\n";
		static constexpr std::string_view kJpegMagicBytes = "\xFF\xD8";
		const std::string baseUrl =
			"https://storage.yandexcloud.net/bridge-bucket/";

		std::string CheckImage(const std::string& image);

		bool ValidateOwner(int postId, int userId);
	};
} // namespace post_service::managers

#endif