#include "Controllers/FriendshipController.hpp"
#include "schemas/UserFriendshipRequest.hpp"

#include <userver/components/component.hpp>
#include <userver/formats/json.hpp>
#include <userver/server/handlers/http_handler_json_base.hpp>
#include <stdexcept>
#include <fmt/format.h>



namespace friendship_service{


    FriendListByTypeHandler::FriendListByTypeHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context) :
    HttpHandlerJsonBase(config, context),
    FriendshipManager_(context.FindComponent<FriendshipManager>()) {}

    userver::formats::json::Value FriendListByTypeHandler::HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json,
        userver::server::request::RequestContext& context) const {

        const auto user_data = request_json.As<friendship_service::models::FriendListDTO>();
        try
        {
            return userver::formats::json::Value AcceptedFriendsJson=FriendshipManager_.TryBuildFriendsListResponse(user_data);
        }
        catch(const std::invalid_argument& e)
        {
            throw userver::server::handlers::BadRequest(userver::server::handlers::ExternalBody{e.what()});
        }
        catch(const std::exception& e)
        {
            throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{fmt::format("Failed while getting friendship list: {}", e.what())});
        }        
    }


    GetStatusFriendshipRelationHandler::GetStatusFriendshipRelationHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context):
    HttpHandlerJsonBase(config, context),
    FriendshipManager_(context.FindComponent<FriendshipManager>()) {}

    userver::formats::json::Value GetStatusFriendshipRelationHandler::HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json,
        userver::server::request::RequestContext& context) const override{
        
        const auto user_data = request_json.As<friendship_service::models::FriendToActionDTO>();

        try{
            return FriendshipManager_.TryGetFriendshipStatus(user_data);
        }
        catch(const std::exception& e){
            throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{fmt::format("Failed while getting friendship status: {}", e.what())});
        }
    }


    RemoveFriendHandler::RemoveFriendHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context) :
    HttpHandlerJsonBase(config, context),
    FriendshipManager_(context.FindComponent<FriendshipManager>()) {}

    userver::formats::json::Value RemoveFriendHandler::HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json,
        userver::server::request::RequestContext& context) const override{
        
        const auto user_data = request_json.As<friendship_service::models::FriendToActionDTO>();

        try{
            return FriendshipManager_.TryRemoveFriend(user_data);
        }
        catch(const std::exception& e){
            throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{fmt::format("Failed while removing friend: {}", e.what())});
        }
    }


    RevokeFriendshipRequestHandler::RevokeFriendshipRequestHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context) :
    HttpHandlerJsonBase(config, context),
    FriendshipManager_(context.FindComponent<FriendshipManager>()) {}

    userver::formats::json::Value RevokeFriendshipRequestHandler::HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json,
        userver::server::request::RequestContext& context) const override{
        
        const auto user_data = request_json.As<friendship_service::models::FriendToActionDTO>();

        try{
            return FriendshipManager_.TryRevokeFriendshipRequest(user_data);
        }
        catch(const std::exception& e){
            throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{fmt::format("Failed while revoking friendship request: {}", e.what())});
        }
    }


    SendFriendshipRequestHandler::SendFriendshipRequestHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context) :
    HttpHandlerJsonBase(config, context),
    FriendshipManager_(context.FindComponent<FriendshipManager>()) {}

    userver::formats::json::Value SendFriendshipRequestHandler::HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json,
        userver::server::request::RequestContext& context) const override{

        const auto user_data = request_json.As<friendship_service::models::FriendToActionDTO>();

        try{
            bool result = FriendshipManager_.TrySendFriendshipRequest(user_data);
        }
        catch(const std::exception& e){
            throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{fmt::format("Failed while sending friend request: {}", e.what())});
        }
    }


    FriendRequestDecisionHandler::FriendRequestDecisionHandler(const userver::components::ComponentConfig& config, const userver::components::ComponentContext& context) :
    HttpHandlerJsonBase(config, context),
    FriendshipManager_(context.FindComponent<FriendshipManager>()) {}

    userver::formats::json::Value FriendRequestDecisionHandler::HandleRequestJsonThrow(
        const userver::server::http::HttpRequest& request,
        const userver::formats::json::Value& request_json,
        userver::server::request::RequestContext& context) const override{

        const auto user_data = request_json.As<friendship_service::models::DecisionFriendRequestDTO>();

        try{
            bool result = FriendshipManager_.TryUpdateFriendRequestStatus(user_data);
        }
        catch(const std::invalid_argument& e){
            throw userver::server::handlers::BadRequest(userver::server::handlers::ExternalBody{e.what()});
        }
        catch(const std::exception& e){
            throw userver::server::handlers::InternalServerError(userver::server::handlers::ExternalBody{fmt::format("Failed while setting friend request decision: {}", e.what())});
        }

    }


    void AddFriendshipController(userver::components::ComponentList& componenr_list)
    {
        component_list
        .Append<friendship_service::FriendListByTypeHandler>()
        .Append<friendship_service::GetStatusFriendshipRelationHandler>()
        .Append<friendship_service::RemoveFriendHandler>()
        .Append<friendship_service::RevokeFriendshipRequestHandler>()
        .Append<friendship_service::SendFriendshipRequestHandler>()
        .Append<friendship_service::FriendRequestDecisionHandler>();
    }

}






