#include "Managers/PostManager.hpp"

#include <userver/components/component.hpp>
#include <userver/s3api/clients/s3api.hpp>
#include <userver/utils/text_light.hpp>
#include <userver/utils/uuid4.hpp>

#include "Managers/S3Manager.hpp"
#include "Repositories/PostRepository.hpp"

namespace post_service::managers
{
	PostManager::PostManager(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: ComponentBase(config, context),
		  s3Client_(context.FindComponent<post_service::managers::S3Manager>()
						.GetClient()),
		  postRepository_(
			  context
				  .FindComponent<post_service::repositories::PostRepository>())
	{
	}

	void PostManager::CreatePost(int userId, const std::string& text,
								 std::optional<std::string>& image)
	{
		std::string fileUrl = "";
		if (image.has_value())
		{
			std::string type = CheckImage(image.value());

			const auto objectKey =
				fmt::format("{}/posts/{}.{}", userId,
							userver::utils::generators::GenerateUuid(), type);
			fileUrl = baseUrl + objectKey;

			s3Client_->PutObject(objectKey, image.value());
		}

		postRepository_.CreatePost(userId, text, fileUrl);
	}

	void PostManager::UpdatePost(int userId, int postId,
								 const std::string& text,
								 std::optional<std::string>& image)
	{
		if (!postRepository_.FindPostById(postId))
			throw std::runtime_error("POST NOT FOUND");
		if (!ValidateOwner(postId, userId)) return;

		std::string fileUrl = "";
		if (image.has_value())
		{
			std::string type = CheckImage(image.value());

			const auto objectKey =
				fmt::format("{}/posts/{}.{}", userId,
							userver::utils::generators::GenerateUuid(), type);
			fileUrl = baseUrl + objectKey;

			s3Client_->PutObject(objectKey, image.value());
		}

		const std::string oldImageUrl =
			postRepository_.UpdatePost(userId, postId, text, fileUrl);

		if (oldImageUrl.size() > 0)
		{
			auto oldObjectKey = oldImageUrl.substr(baseUrl.size());
			s3Client_->DeleteObject(oldObjectKey);
		}
	}

	void PostManager::DeletePost(int userId, int postId)
	{
		if (!postRepository_.FindPostById(postId))
			throw std::runtime_error("POST NOT FOUND");
		if (!ValidateOwner(postId, userId)) return;

		std::string filePath = postRepository_.DeletePost(userId, postId);
		if (filePath.size() > 0)
		{
			auto objectKey = filePath.substr(baseUrl.size());
			s3Client_->DeleteObject(objectKey);
		}
	}

	std::pair<int, bool> PostManager::ToggleLike(int userId, int postId)
	{
		if (!postRepository_.FindPostById(postId))
			throw std::runtime_error("POST NOT FOUND");
		if (postRepository_.HasLike(userId, postId))
		{
			int count = postRepository_.RemoveLike(userId, postId);
			return {count - 1, false};
		}
		else
		{
			int count = postRepository_.AddLike(userId, postId);
			return {count + 1, true};
		}
	}

	post_service::models::Post PostManager::GetPost(int postId)
	{
		auto res = postRepository_.GetPost(postId);
		if (res.has_value())
			return res.value();
		else
			throw std::runtime_error("POST NOT FOUND");
	}

	std::vector<post_service::models::Post> PostManager::GetUserPosts(
		int userId)
	{
		auto res = postRepository_.GetPosts(userId);
		if (res.has_value())
			return res.value();
		else
			return {};
	}

	std::string PostManager::CheckImage(const std::string& image)
	{
		std::string type = "";
		if (userver::utils::text::StartsWith(image, kPngMagicBytes))
			type = "png";
		else if (userver::utils::text::StartsWith(image, kJpegMagicBytes))
			type = "jpeg";
		else
			throw std::runtime_error("UNSUPPORTED_TYPE");

		return type;
	}

	bool PostManager::ValidateOwner(int postId, int userId)
	{
		return postRepository_.CheckOwner(postId, userId);
	}

} // namespace post_service::managers