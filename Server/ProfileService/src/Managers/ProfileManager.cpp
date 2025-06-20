#include "Managers/ProfileManager.hpp"

#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/logging/log.hpp>
#include <userver/s3api/clients/s3api.hpp>
#include <userver/utils/text_light.hpp>
#include <userver/utils/uuid4.hpp>

#include "Managers/S3Manager.hpp"
#include "Repositories/ProfileRepository.hpp"

namespace profile_service::managers
{
	ProfileManager::ProfileManager(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: ComponentBase(config, context),
		  s3Client_(
			  context.FindComponent<profile_service::managers::S3Manager>()
				  .GetClient()),
		  profileRepository_(
			  context.FindComponent<
				  profile_service::repositories::ProfileRepository>())
	{
	}

	std::string ProfileManager::UpdateAvatar(int id, const std::string& image)
	{
		std::string type = "";
		if (userver::utils::text::StartsWith(image, kPngMagicBytes))
			type = "png";
		else if (userver::utils::text::StartsWith(image, kJpegMagicBytes))
			type = "jpeg";
		else
			throw std::runtime_error("UNSUPPORTED_TYPE");

		const auto objectKey = fmt::format(
			"avatars/{}.{}", userver::utils::generators::GenerateUuid(), type);
		const auto fileUrl = baseUrl + objectKey;

		s3Client_->PutObject(objectKey, image);

		auto oldImagePath = profileRepository_.UpdateAvatar(id, fileUrl);

		auto oldObjectKey = oldImagePath.substr(baseUrl.size());
		if (oldObjectKey != "avatars/default.jpg")
			s3Client_->DeleteObject(oldObjectKey);
		return fileUrl;
	}

	std::optional<profile_service::models::ProfileInfo> ProfileManager::GetInfo(
		int id)
	{
		return profileRepository_.GetProfileById(id);
	}

	profile_service::models::ProfileInfo ProfileManager::UpdateInfo(
		int id, profile_service::models::ProfileUpdateDTO profileInfo)
	{
		return profileRepository_.UpdateInfo(
			id, profileInfo.first_name.value_or(""),
			profileInfo.last_name.value_or(""),
			profileInfo.username.value_or(""));
	}

} // namespace profile_service::managers