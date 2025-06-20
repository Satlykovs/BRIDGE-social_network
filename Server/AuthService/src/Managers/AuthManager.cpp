#include "Managers/AuthManager.hpp"

#include "Managers/JwtManager.hpp"
// #include "Managers/KafkaProducer.hpp"

#include <bcrypt/BCrypt.hpp>
#include <optional>
#include <string>
#include <userver/components/component.hpp>
#include <userver/utils/datetime/timepoint_tz.hpp>
#include <userver/utils/optionals.hpp>
namespace auth_service::managers
{
	AuthManager::AuthManager(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: ComponentBase(config, context),
		  authRepository_(
			  context
				  .FindComponent<auth_service::repositories::AuthRepository>()),
		  jwtManager_(
			  context.FindComponent<auth_service::managers::JwtManager>())
	// kafkaProducer_(context.FindComponent<auth_service::managers::Producer>())
	{
	}

	void AuthManager::RegisterUser(auth_service::models::UserDTO& userData)
	{
		if (authRepository_.FindUserByEmail(userData.email) != std::nullopt)
		{
			throw std::runtime_error("USER EXISTS");
		}
		ValidatePassword(userData.password);
		std::string passwordHash = HashPassword(userData.password);

		authRepository_.CreateUser(userData.email, passwordHash);
	}

	std::pair<std::pair<std::string, userver::utils::datetime::TimePointTz>,
			  std::pair<std::string, userver::utils::datetime::TimePointTz>>
	AuthManager::AuthenticateUser(auth_service::models::UserDTO& userData)
	{
		auto user = authRepository_.FindUserByEmail(userData.email);
		if (user == std::nullopt)
		{
			throw std::runtime_error(
				fmt::format("User {} not found.", userData.email));
		}

		ValidatePassword(userData.password);

		if (!VerifyPassword(userData.password, user.value().password_hash))
		{
			throw std::runtime_error("Invalid password");
		}
		return std::make_pair(
			jwtManager_.GenerateAccessToken(user.value().id),
			jwtManager_.GenerateRefreshToken(user.value().id));
	}

	std::string HashPassword(const std::string& password)
	{
		return BCrypt::generateHash(password);
	}

	bool VerifyPassword(const std::string& password, const std::string& hash)
	{
		return BCrypt::validatePassword(password, hash);
	}

	bool IsValidPassword(const std::string& password)
	{
		if (password.size() < 8)
		{
			return false;
		}

		bool hasUpper = false;
		bool hasLower = false;
		bool hasDigit = false;
		bool hasSpecial = false;

		const std::string SPECIAL_CHARACTERS = "#?!@$%^&*-";

		for (char c : password)
		{
			if (std::isupper(c))
			{
				hasUpper = true;
			}
			else if (std::islower(c))
			{
				hasLower = true;
			}
			else if (std::isdigit(c))
			{
				hasDigit = true;
			}
			else if (SPECIAL_CHARACTERS.find(c) != std::string::npos)
			{
				hasSpecial = true;
			}
		}

		return hasUpper && hasLower && hasDigit && hasSpecial;
	}

	void ValidatePassword(const std::string& password)
	{
		if (!IsValidPassword(password))
		{
			throw std::invalid_argument(
				"Password does not meet the requirements. "
				"It must be at least 8 characters long and contain "
				"at least one uppercase letter, one lowercase letter, "
				"one digit, and one special character from the set: "
				"#?!@$%^&*-");
		}
	}
} // namespace auth_service::managers