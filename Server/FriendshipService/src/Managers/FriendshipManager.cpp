#include "Managers/FriendshipManager.hpp"

#include <fmt/format.h>

#include <stdexcept>
#include <string>
#include <userver/formats/json/value_builder.hpp>
#include <userver/storages/postgres/result_set.hpp>

#include "Repositories/FriendshipRepository.hpp"

namespace friendship_service
{

	FriendshipManager::FriendshipManager(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: userver::components::ComponentBase(config, context),
		  FriendshipRepository_(context.FindComponent<FriendshipRepository>())
	{
	}

	userver::formats::json::Value
	FriendshipManager::TryBuildFriendsListResponse(
		const friendship_service::models::FriendListDTO& user_data,
		int currentUserID)
	{
		// int currentUserID = user_data.currentUserID;
		std::string friendListType = user_data.friendListType;

		userver::storages::postgres::ResultSet result =
			FriendshipRepository_.GetFriendsListQuery(currentUserID,
													  friendListType);

		userver::formats::json::ValueBuilder builder;
		builder["user_id"] = currentUserID;
		builder["ids"] = userver::formats::common::Type::kArray;

		for (const auto& row : result)
		{
			builder["ids"].PushBack(row["friend_id"].As<int>());
		}
		return builder.ExtractValue();
	}

	userver::formats::json::Value FriendshipManager::TryGetFriendshipStatus(
		const friendship_service::models::FriendToActionDTO& user_data,
		int currentUserID)
	{
		// int currentUserID = user_data.currentUserID;
		int friendToGetStatusID = user_data.friendToActionID;

		userver::storages::postgres::ResultSet result =
			FriendshipRepository_.FriendshipStatusQuery(currentUserID,
														friendToGetStatusID);

		userver::formats::json::ValueBuilder builder;
		builder["user_id"] = currentUserID;

		if (result.IsEmpty())
		{
			builder["friendship_status"] =
				"It is possible to send a friend request.";
			return builder.ExtractValue();
		}

		std::string status = result[0]["status"].As<std::string>();

		int senderID = result[0]["sender_id"].As<int>();

		if (status == "accepted")
		{
			builder["friendship_status"] =
				"Added to friends. It is possible to remove friend.";
		}
		else if (status == "sent" && currentUserID == senderID)
		{
			builder["friendship_status"] =
				"Friend request sent. It is possible to revoke a friendship "
				"request.";
		}
		else if (status == "sent" && currentUserID != senderID)
		{
			builder["friendship_status"] =
				"Friend request received. It is possible to "
				"make a decision about the friend request";
		}
		else
		{
			throw std::runtime_error(
				fmt::format("Unexpected friendship status: {}", status));
		}

		return builder.ExtractValue();
	}

	userver::formats::json::Value FriendshipManager::TryRemoveFriend(
		const friendship_service::models::FriendToActionDTO& user_data,
		int currentUserID)
	{
		// int currentUserID = user_data.currentUserID;
		int friendToRemoveID = user_data.friendToActionID;

		userver::storages::postgres::ResultSet result =
			FriendshipRepository_.RemoveFriendQuery(currentUserID,
													friendToRemoveID);

		userver::formats::json::ValueBuilder builder;
		builder["user_id"] = currentUserID;

		size_t removeFriendsCount = result.RowsAffected();

		if (removeFriendsCount == 1 || removeFriendsCount == 0)
		{
			builder["remove_friends_count"] = removeFriendsCount;
		}
		else
		{
			throw std::runtime_error(
				fmt::format("Unexpected number of removed friends: {}",
							removeFriendsCount));
		}

		return builder.ExtractValue();
	}

	userver::formats::json::Value FriendshipManager::TryRevokeFriendshipRequest(
		const friendship_service::models::FriendToActionDTO& user_data,
		int currentUserID)
	{
		// int currentUserID = user_data.currentUserID;
		int receiverID = user_data.friendToActionID;

		userver::storages::postgres::ResultSet result =
			FriendshipRepository_.RevokeFriendshipRequestQuery(currentUserID,
															   receiverID);

		userver::formats::json::ValueBuilder builder;
		builder["user_id"] = currentUserID;

		size_t revokeFriendshipRequestsCount = result.RowsAffected();

		if (revokeFriendshipRequestsCount == 1 ||
			revokeFriendshipRequestsCount == 0)
		{
			builder["revoke_friendship_requests_count"] =
				revokeFriendshipRequestsCount;
		}
		else
		{
			throw std::runtime_error(fmt::format(
				"Unexpected number of revoked friendship requests: {}",
				revokeFriendshipRequestsCount));
		}

		return builder.ExtractValue();
	}

	userver::formats::json::Value FriendshipManager::TrySendFriendshipRequest(
		const friendship_service::models::FriendToActionDTO& user_data,
		int currentUserID)
	{
		// int currentUserID = user_data.currentUserID;
		int receiverID = user_data.friendToActionID;

		if (currentUserID == receiverID)
		{
			throw std::invalid_argument(
				"It is not possible to send a friend request to yourself");
		}

		userver::storages::postgres::ResultSet currentFriendshipRelationStatus =
			FriendshipRepository_.FriendshipStatusQuery(currentUserID,
														receiverID);

		userver::formats::json::ValueBuilder builder;
		builder["user_id"] = currentUserID;

		if (currentFriendshipRelationStatus.IsEmpty())
		{
			userver::storages::postgres::ResultSet result =
				FriendshipRepository_.SendFriendshipRequestQuery(currentUserID,
																 receiverID);
			if (result.RowsAffected() == 1)
			{
				builder["sending_status"] = "Successfully sent!";
				return builder.ExtractValue();
			}
			else
			{
				throw std::runtime_error(
					"Unexpected number of added friend requests!");
			}
		}
		else if (currentFriendshipRelationStatus[0]["status"]
					 .As<std::string>() == "accepted")
		{
			builder["sending_status"] = "This user is already your friend!";
			return builder.ExtractValue();
		}
		else if (currentUserID ==
				 currentFriendshipRelationStatus[0]["sender_id"].As<int>())
		{
			builder["sending_status"] = "Friend request has already been sent!";
			return builder.ExtractValue();
		}
		else if (currentUserID ==
				 currentFriendshipRelationStatus[0]["receiver_id"].As<int>())
		{
			FriendshipRepository_.UpdateFriendRequestStatusQuery(
				currentUserID,
				currentFriendshipRelationStatus[0]["sender_id"].As<int>(),
				true);
			builder["sending_status"] =
				"Friend request was successfully accepted";
			return builder.ExtractValue();
		}
		else
		{
			throw std::runtime_error("Unexpected friendship relation status!");
		}
	}

	userver::formats::json::Value
	FriendshipManager::TryUpdateFriendRequestStatus(
		const friendship_service::models::DecisionFriendRequestDTO& user_data,
		int currentUserID)
	{
		// int currentUserID = user_data.currentUserID;
		int senderID = user_data.senderID;
		bool decision = user_data.decision;

		userver::storages::postgres::ResultSet result =
			FriendshipRepository_.UpdateFriendRequestStatusQuery(
				currentUserID, senderID, decision);

		size_t FriendshipRequestsUpdated = result.RowsAffected();

		userver::formats::json::ValueBuilder builder;
		builder["user_id"] = currentUserID;

		switch (FriendshipRequestsUpdated)
		{
			case 0:
			{
				builder["request_success"] = "FALSE";
				return builder.ExtractValue();
			}
			case 1:
			{
				builder["request_success"] = "TRUE";
				return builder.ExtractValue();
			}
			default:
			{
				throw std::runtime_error(
					fmt::format("Query affected {} rows (expected 1)",
								FriendshipRequestsUpdated));
			}
		}
	}

} // namespace friendship_service
