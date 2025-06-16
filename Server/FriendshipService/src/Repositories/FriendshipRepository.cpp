#include "Repositories/FriendshipRepository.hpp"

#include <fmt/format.h>

#include <stdexcept>
#include <string>
#include <userver/components/component.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/date.hpp>

#include "schemas/UserFriendshipRequest.hpp"
#include "sql/include/sql_queries/sql_queries.hpp"

namespace friendship_service
{

	FriendshipRepository::FriendshipRepository(
		const userver::components::ComponentConfig& config,
		const userver::components::ComponentContext& context)
		: ComponentBase(config, context),
		  pgCluster_(
			  context
				  .FindComponent<userver::components::Postgres>("friendship-db")
				  .GetCluster())
	{
		// pgCluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster,sql_queries::sql::kCreateTable);
	}

	userver::storages::postgres::ResultSet
	FriendshipRepository::GetFriendsListQuery(int currentUserID,
											  const std::string& friendListType)
	{
		if (friendListType == "CurrentFriendsList")
		{
			return pgCluster_->Execute(
				userver::storages::postgres::ClusterHostType::kMaster,
				sql_queries::sql::kGetCurrentFriends, currentUserID);
		}
		else if (friendListType == "ReceivedFriendRequestsList")
		{
			return pgCluster_->Execute(
				userver::storages::postgres::ClusterHostType::kMaster,
				sql_queries::sql::kGetReceivedFriendRequests, currentUserID);
		}
		else if (friendListType == "SentFriendRequestsList")
		{
			return pgCluster_->Execute(
				userver::storages::postgres::ClusterHostType::kMaster,
				sql_queries::sql::kGetSentFriendRequests, currentUserID);
		}
		else
		{
			throw std::invalid_argument(
				fmt::format("Invalid friend list type: {}", friendListType));
		}
	}

	userver::storages::postgres::ResultSet
	FriendshipRepository::FriendshipStatusQuery(int currentUserID,
												int friendToGetStatusID)
	{
		return pgCluster_->Execute(
			userver::storages::postgres::ClusterHostType::kMaster,
			sql_queries::sql::kGetStatusFriendshipRelation, currentUserID,
			friendToGetStatusID);
	}

	userver::storages::postgres::ResultSet
	FriendshipRepository::RemoveFriendQuery(int currentUserID,
											int friendToRemoveID)
	{
		return pgCluster_->Execute(
			userver::storages::postgres::ClusterHostType::kMaster,
			sql_queries::sql::kRemoveFriend, currentUserID, friendToRemoveID);
	}

	userver::storages::postgres::ResultSet
	FriendshipRepository::RevokeFriendshipRequestQuery(int currentUserID,
													   int receiverID)
	{
		return pgCluster_->Execute(
			userver::storages::postgres::ClusterHostType::kMaster,
			sql_queries::sql::kRevokeFriendshipRequest, currentUserID,
			receiverID);
	}

	userver::storages::postgres::ResultSet
	FriendshipRepository::SendFriendshipRequestQuery(int currentUserID,
													 int receiverID)
	{
		return pgCluster_->Execute(
			userver::storages::postgres::ClusterHostType::kMaster,
			sql_queries::sql::kSendFriendshipRequest, currentUserID,
			receiverID);
	}

	userver::storages::postgres::ResultSet
	FriendshipRepository::UpdateFriendRequestStatusQuery(int currentUserID,
														 int senderID,
														 bool decision)
	{
		if (decision)
		{
			return pgCluster_->Execute(
				userver::storages::postgres::ClusterHostType::kMaster,
				sql_queries::sql::kUpdateFriendRequestStatus, currentUserID,
				senderID);
		}
		else
		{
			return pgCluster_->Execute(
				userver::storages::postgres::ClusterHostType::kMaster,
				sql_queries::sql::kRevokeFriendshipRequest, senderID,
				currentUserID);
		}
	}

} // namespace friendship_service