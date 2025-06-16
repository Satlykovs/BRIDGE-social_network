#ifndef FRIENDSHIP_REPOSITORY_HPP
#define FRIENDSHIP_REPOSITORY_HPP

#include <userver/components/component_fwd.hpp>
#include <userver/formats/json.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>

#include "schemas/UserFriendshipRequest.hpp"

namespace friendship_service
{
	class FriendshipRepository final : public userver::components::ComponentBase
	{
	   public:
		static constexpr std::string_view kName = "friendship-storage";

		FriendshipRepository(
			const userver::components::ComponentConfig& config,
			const userver::components::ComponentContext& context);

		userver::storages::postgres::ResultSet GetFriendsListQuery(
			int currentUserID, const std::string& friendListType);

		userver::storages::postgres::ResultSet FriendshipStatusQuery(
			int currentUserID, int friendToGetStatusID);

		userver::storages::postgres::ResultSet RemoveFriendQuery(
			int currentUserID, int friendToRemoveID);

		userver::storages::postgres::ResultSet RevokeFriendshipRequestQuery(
			int currentUserID, int receiverID);

		userver::storages::postgres::ResultSet SendFriendshipRequestQuery(
			int currentUserID, int receiverID);

		userver::storages::postgres::ResultSet UpdateFriendRequestStatusQuery(
			int currentUserID, int senderID, bool decision);

	   private:
		userver::storages::postgres::ClusterPtr pgCluster_;
	};
} // namespace friendship_service
#endif