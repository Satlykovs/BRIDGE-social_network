#include <userver/clients/dns/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>

#include "include/Handlers/Http/CreateConversation.hpp"         
#include "include/Handlers/Http/ListConversationsHandler.hpp"  
#include "include/Handlers/Http/SendMessageHandler.hpp"        
#include "include/Handlers/Http/GetMessagesHandler.hpp"        
#include "include/Handlers/Http/MarkAsReadHandler.hpp"         
#include "include/Handlers/Http/UpdateMessageHandler.hpp"      
#include "include/Handlers/Http/DeleteMessageHandler.hpp"      
#include "include/Handlers/Http/AddParticipantHandler.hpp"     
#include "include/Handlers/Http/RemoveParticipantHandler.hpp"  
#include "include/Handlers/Http/LeaveConversationHandler.hpp"  
#include "include/Handlers/Http/UpdateConversationHandler.hpp" 
#include "include/Handlers/Http/GetConversationDetailsHandler.hpp" 

int main(int argc, char* argv[]) {
    auto component_list =
        userver::components::MinimalServerComponentList()
            .Append<userver::components::TestsuiteSupport>()
            .Append<userver::clients::dns::Component>()
            .Append<userver::components::Postgres>("chat-db") 

            .Append<chat_service::http_handlers::CreateConversationHandler>()
            .Append<chat_service::http_handlers::ListConversationsHandler>()
            .Append<chat_service::http_handlers::SendMessageHandler>() 
            .Append<chat_service::http_handlers::GetMessagesHandler>() 
            .Append<chat_service::http_handlers::MarkAsReadHandler>()
            .Append<chat_service::http_handlers::UpdateMessageHandler>()
            .Append<chat_service::http_handlers::DeleteMessageHandler>()
            .Append<chat_service::http_handlers::AddParticipantHandler>()
            .Append<chat_service::http_handlers::RemoveParticipantHandler>()
            .Append<chat_service::http_handlers::LeaveConversationHandler>()
            .Append<chat_service::http_handlers::UpdateConversationHandler>()
            .Append<chat_service::http_handlers::GetConversationDetailsHandler>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}

