#ifndef NETUTIL_H
#define NETUTIL_H 

#include <typeindex>
#include <string>

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

#include "enet/enet.h" 

namespace std 
{ 

    template<>
    struct hash<ENetAddress> { 
        
        std::size_t operator()(ENetAddress& address) const noexcept 
        {
            std::string formatted = std::to_string(address.host) + ":" + std::to_string(address.port); 
            return std::hash<std::string>()(formatted); 
        }

    };

}

#endif // NETUTIL_H