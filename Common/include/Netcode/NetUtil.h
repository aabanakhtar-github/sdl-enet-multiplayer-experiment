#ifndef NETUTIL_H
#define NETUTIL_H

#include <string>

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include "Util.h"
#include "enet/enet.h"

namespace std {

template <> struct hash<ENetAddress> {

  size_t operator()(const ENetAddress &address) const noexcept {
    size_t seed = hash<int>()(address.host);
    hash_combine<size_t>(seed, address.port);
    return seed;
  }
};

} // namespace std

inline bool checkIP(const std::string &ip) {
  for (auto c : ip) {
    if (!std::isdigit(c) || c == '.') {
      return false;
    }
  }
  // good
  return true;
}

inline bool checkPort(const std::string &port) {
  for (auto c : port) {
    if (!isdigit(c)) {
      return false;
    }
  }
  // good
  return true;
}

#endif // NETUTIL_H
