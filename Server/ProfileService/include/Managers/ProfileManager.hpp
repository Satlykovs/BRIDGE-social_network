#ifndef PROFILE_MANAGER_HPP
#define PROFILE_MANAGER_HPP

#include <userver/components/component_base.hpp>
#include <userver/s3api/clients/fwd.hpp>

#include "Repositories/ProfileRepository.hpp"
#include "schemas/Profile.hpp"

namespace profile_service::managers
{
	class ProfileManager final : public userver::components::ComponentBase
	{
	   public:
		static constexpr std::string_view kName = "profile-manager";

		ProfileManager(const userver::components::ComponentConfig& config,
					   const userver::components::ComponentContext& context);

		std::string UpdateAvatar(int id, const std::string& image);

		profile_service::models::ProfileInfo UpdateInfo(
			int id, profile_service::models::ProfileUpdateDTO profileInfo);

		std::optional<profile_service::models::ProfileInfo> GetInfo(int id);

	   private:
		userver::s3api::ClientPtr s3Client_;
		profile_service::repositories::ProfileRepository& profileRepository_;
		static constexpr std::string_view kPngMagicBytes = "\x89PNG\r\n\x1a\n";
		static constexpr std::string_view kJpegMagicBytes = "\xFF\xD8";
		const std::string baseUrl =
			"https://storage.yandexcloud.net/bridge-bucket/";
	};
} // namespace profile_service::managers

#endif