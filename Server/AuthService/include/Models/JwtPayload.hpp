#ifndef JWT_PAYLOAD_HPP
#define JWT_PAYLOAD_HPP

#include <string>

namespace auth_service
{

struct JwtPayload
{
    std::string email;
    
};

}
#endif