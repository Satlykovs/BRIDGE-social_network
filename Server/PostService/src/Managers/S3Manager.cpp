#include "Managers/S3Manager.hpp"

#include <userver/clients/http/component.hpp>
#include <userver/components/component.hpp>
#include <userver/components/component_base.hpp>
#include <userver/s3api/clients/s3api.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace post_service::managers
{
	S3Manager::S3Manager(const userver::components::ComponentConfig& config,
						 const userver::components::ComponentContext& context)
		: ComponentBase(config, context),
		  httpClient_(context.FindComponent<userver::components::HttpClient>()
						  .GetHttpClient()),
		  accessKey_(config["access-key"].As<std::string>()),
		  secretKey_(config["secret-key"].As<std::string>())
	{
	}

	userver::s3api::ClientPtr S3Manager::GetClient()
	{
		auto connectionConfig = userver::s3api::ConnectionCfg(
			std::chrono::milliseconds{1000}, 2, std::nullopt);

		auto s3Connection = userver::s3api::MakeS3Connection(
			httpClient_, userver::s3api::S3ConnectionType::kHttps,
			"storage.yandexcloud.net", connectionConfig);

		auto auth = std::make_shared<userver::s3api::authenticators::AccessKey>(
			accessKey_, userver::s3api::Secret(secretKey_));

		return userver::s3api::GetS3Client(s3Connection, auth, "bridge-bucket");
	}

	userver::yaml_config::Schema S3Manager::GetStaticConfigSchema()
	{
		return userver::yaml_config::MergeSchemas<
			userver::components::ComponentBase>(R"(
            type: object
            description: S3 Manager component
            additionalProperties: true
            properties:
                access-key:
                    type: string
                    description: Access key of s3 object storage
                secret-key:
                    type: string
                    description: Secret key of s3 object storage
            )");
	}
} // namespace post_service::managers