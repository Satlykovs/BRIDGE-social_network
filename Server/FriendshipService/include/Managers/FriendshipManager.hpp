#ifndef FRIENDSHIP_MANAGER_HPP
#define FRIENDSHIP_MANAGER_HPP

#include <userver/components/component_context.hpp>
#include "Repositories/FriendshipRepository.hpp"
#include <userver/components/component_fwd.hpp>

namespace friendship_service
{

    class FriendshipManager final: public userver::components::ComponentBase
    {
        public:
            static constexpr std::string_view kName = "friendship-manager";

            FriendshipManager(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context);

            userver::formats::json::Value TryBuildFriendsListResponse(const friendship_service::models::FriendListDTO& user_data,int currentUserID);

            userver::formats::json::Value TryGetFriendshipStatus(const friendship_service::models::FriendToActionDTO& user_data,int currentUserID);

            userver::formats::json::Value TryRemoveFriend(const friendship_service::models::FriendToActionDTO& user_data,int currentUserID);

            userver::formats::json::Value TryRevokeFriendshipRequest(const friendship_service::models::FriendToActionDTO& user_data,int currentUserID);

            userver::formats::json::Value TrySendFriendshipRequest(const friendship_service::models::FriendToActionDTO& user_data,int currentUserID);

            userver::formats::json::Value TryUpdateFriendRequestStatus(const friendship_service::models::DecisionFriendRequestDTO& user_data,int currentUserID);

        private:


            friendship_service::FriendshipRepository& FriendshipRepository_;

    };
    

}


#endif