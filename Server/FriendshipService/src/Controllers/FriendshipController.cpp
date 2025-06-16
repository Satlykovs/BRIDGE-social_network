#include "Controllers/FriendshipController.hpp"

#include <fmt/format.h>

#include <stdexcept>
#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/exceptions.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>

#include "schemas/UserFriendshipRequest.hpp"

namespace friendship_service
{

	FriendListByTypeHandler::FriendListByTypeHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  FriendshipManager_(context.FindComponent<FriendshipManager>())
	{
	}

	userver::formats::json::Value
	FriendListByTypeHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& request_json,
		userver::server::request::RequestContext& context) const
	{
		const auto user_data =
			request_json.As<friendship_service::models::FriendListDTO>();
		try
		{
			int CurrentUserID = context.GetData<int>("user_id");
			return FriendshipManager_.TryBuildFriendsListResponse(
				user_data, CurrentUserID);
		}
		catch (const std::invalid_argument& e)
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{e.what()});
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::InternalServerError(
				userver::server::handlers::ExternalBody{fmt::format(
					"Failed while getting friendship list: {}", e.what())});
		}
	}

	GetStatusFriendshipRelationHandler::GetStatusFriendshipRelationHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  FriendshipManager_(context.FindComponent<FriendshipManager>())
	{
	}

	userver::formats::json::Value
	GetStatusFriendshipRelationHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& request_json,
		userver::server::request::RequestContext& context) const
	{
		const auto user_data =
			request_json.As<friendship_service::models::FriendToActionDTO>();

		try
		{
			int CurrentUserID = context.GetData<int>("user_id");
			return FriendshipManager_.TryGetFriendshipStatus(user_data,
															 CurrentUserID);
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::InternalServerError(
				userver::server::handlers::ExternalBody{fmt::format(
					"Failed while getting friendship status: {}", e.what())});
		}
	}

	RemoveFriendHandler::RemoveFriendHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  FriendshipManager_(context.FindComponent<FriendshipManager>())
	{
	}

	userver::formats::json::Value RemoveFriendHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& request_json,
		userver::server::request::RequestContext& context) const
	{
		const auto user_data =
			request_json.As<friendship_service::models::FriendToActionDTO>();

		try
		{
			int CurrentUserID = context.GetData<int>("user_id");
			return FriendshipManager_.TryRemoveFriend(user_data, CurrentUserID);
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::InternalServerError(
				userver::server::handlers::ExternalBody{
					fmt::format("Failed while removing friend: {}", e.what())});
		}
	}

	RevokeFriendshipRequestHandler::RevokeFriendshipRequestHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  FriendshipManager_(context.FindComponent<FriendshipManager>())
	{
	}

	userver::formats::json::Value
	RevokeFriendshipRequestHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& request_json,
		userver::server::request::RequestContext& context) const
	{
		const auto user_data =
			request_json.As<friendship_service::models::FriendToActionDTO>();

		try
		{
			int CurrentUserID = context.GetData<int>("user_id");
			return FriendshipManager_.TryRevokeFriendshipRequest(user_data,
																 CurrentUserID);
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::InternalServerError(
				userver::server::handlers::ExternalBody{fmt::format(
					"Failed while revoking friendship request: {}", e.what())});
		}
	}

	SendFriendshipRequestHandler::SendFriendshipRequestHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  FriendshipManager_(context.FindComponent<FriendshipManager>())
	{
	}

	userver::formats::json::Value
	SendFriendshipRequestHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& request_json,
		userver::server::request::RequestContext& context) const
	{
		const auto user_data =
			request_json.As<friendship_service::models::FriendToActionDTO>();

		try
		{
			int CurrentUserID = context.GetData<int>("user_id");
			return FriendshipManager_.TrySendFriendshipRequest(user_data,
															   CurrentUserID);
		}
		catch (const std::invalid_argument& e)
		{
			throw userver::server::handlers::ClientError(
				userver::server::handlers::ExternalBody{fmt::format(
					"Failed while sending friend request: {}", e.what())});
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::InternalServerError(
				userver::server::handlers::ExternalBody{fmt::format(
					"Failed while sending friend request: {}", e.what())});
		}
	}

	FriendshipRequestDecisionHandler::FriendshipRequestDecisionHandler(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: HttpHandlerJsonBase(config, context),
		  FriendshipManager_(context.FindComponent<FriendshipManager>())
	{
	}

	userver::formats::json::Value
	FriendshipRequestDecisionHandler::HandleRequestJsonThrow(
		const userver::server::http::HttpRequest& request,
		const userver::formats::json::Value& request_json,
		userver::server::request::RequestContext& context) const
	{
		const auto user_data =
			request_json
				.As<friendship_service::models::DecisionFriendRequestDTO>();

		try
		{
			int CurrentUserID = context.GetData<int>("user_id");
			return FriendshipManager_.TryUpdateFriendRequestStatus(
				user_data, CurrentUserID);
		}
		catch (const std::exception& e)
		{
			throw userver::server::handlers::InternalServerError(
				userver::server::handlers::ExternalBody{fmt::format(
					"Failed while setting friend request decision: {}",
					e.what())});
		}
	}

	void AddFriendshipController(
		userver::components::ComponentList& component_list)
	{
		component_list.Append<friendship_service::FriendListByTypeHandler>()
			.Append<friendship_service::GetStatusFriendshipRelationHandler>()
			.Append<friendship_service::RemoveFriendHandler>()
			.Append<friendship_service::RevokeFriendshipRequestHandler>()
			.Append<friendship_service::SendFriendshipRequestHandler>()
			.Append<friendship_service::FriendshipRequestDecisionHandler>();
	}

} // namespace friendship_service
