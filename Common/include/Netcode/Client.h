#ifndef CLIENT_H
#define CLIENT_H

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include "Packet.h"
#include "enet/enet.h"
#include <functional>

struct ServerInfo {
  ENetPeer *server;
  ENetAddress address;
};

class NetClient {
  NetClient(const NetClient &) = delete;
  NetClient &operator=(const NetClient &) = delete;

public:
  // TODO: add copy and swap to make cleaner code / easy to update
  NetClient();
  explicit NetClient(const std::string &ip, std::uint16_t port,
                     std::function<void(const PacketData &)> recv_callback);
  NetClient(NetClient &&) noexcept;
  NetClient &operator=(NetClient &&) noexcept;
  ~NetClient();

  void updateNetwork(const float block_time = 0.0,
                     const bool disconnection = false);
  void connectTo(const std::string &server_addr, const std::uint16_t port,
                 const float timeout);
  void disconnect(const float timeout);
  void sendPacket(PacketData &packet, const int channel, bool reliable = false);

  [[nodiscard]] bool getConnected() const { return valid_ && connected_; }
  [[nodiscard]] bool getValid() const { return valid_; }
  [[nodiscard]] int getID() const { return ID_; }

  friend void swap(NetClient &a, NetClient &b) {
    using std::swap;
    swap(a.recv_callback_, b.recv_callback_);
    swap(a.client_, b.client_);
    swap(a.server_, b.server_);
    swap(a.username_, b.username_);
    swap(a.ID_, b.ID_);
    swap(a.valid_, b.valid_);
    swap(a.connected_, b.connected_);
    swap(a.client_salt_, b.client_salt_);
    swap(a.server_salt_, b.server_salt_);
  }

private:
  std::function<void(const PacketData &)> recv_callback_;
  ENetHost *client_;
  ServerInfo server_;
  std::string username_;
  bool valid_ = false;
  bool connected_ = false;
  int ID_ = -1;
  int server_salt_ = -1;
  int client_salt_ = -1;
};

#endif
