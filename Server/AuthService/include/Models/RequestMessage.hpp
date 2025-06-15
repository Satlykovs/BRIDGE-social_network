#ifndef REQUEST_MESSAGE_HPP
#define REQUEST_MESSAGE_HPP

#include <string>
#include <userver/formats/json/value.hpp>
namespace auth_service::models
{
    enum class SendStatus {kSuccess, kErrorRetryable, kErrorNonRetryable};

    struct RequestMessage
    {
        std::string topic;
        std::string key;
        std::string payload;

        

    };
}
#endif