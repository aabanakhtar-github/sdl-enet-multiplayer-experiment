#ifndef NETUTIL_H
#define NETUTIL_H

#include <string>

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include "enet/enet.h"
#include "Util.h"

namespace std {

template <> struct hash<ENetAddress> {

  size_t operator()(const ENetAddress &address) const noexcept {
    size_t seed = hash<int>()(address.host); 
    hash_combine<size_t>(seed, address.port);
    return seed;  
  }
};

} // namespace std

#endif // NETUTIL_H
