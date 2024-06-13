#ifndef SERVER_H 
#define SERVER_H 

#include <cstdint> 
#include <unordered_map>
#include <queue> 
#include <functional>
#include <any> 

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

#include "enet/enet.h"
#include "Packet.h"
#include "NetUtil.h"

struct ServerClientInfo : ClientInfo {
    ENetPeer* peer = nullptr; 
    int client_salt = -1;
    int server_salt = -1;
    std::size_t hash;
    std::any user_data; 
}; 

class NetServer {
public:
    NetServer() : server_(nullptr) {}
    explicit NetServer(std::uint16_t port, std::size_t peers, std::function<void(const PacketData&)> recv_callback);
    ~NetServer(); 
    NetServer(NetServer&&) noexcept;
    NetServer& operator = (NetServer&&) noexcept;
    NetServer(const NetServer&) = delete;
    NetServer& operator = (const NetServer&) = delete;

    void sendPacketToPending(const PacketData& packet, std::size_t hash, int channel, bool reliable = false);
    void sendPacketTo(PacketData& packet, std::size_t ID, int channel, bool reliable = false);
    void broadcastPacket(PacketData& packet, int channel, bool reliable = false);
    void broadcastPacketToAllExcept(PacketData& packet, int channel, std::size_t ID, bool reliable = false);
    void updateNetwork(float block_time = 0.0f); 

    bool getValid() const { return valid_; }
    // returns a map of network IDs to their info
    // std::pair < network id, client data >
    std::unordered_map<std::size_t, ServerClientInfo>& GetClients() { return clients_; }

    friend void swap(NetServer& a, NetServer& b) { 
        using std::swap;
        swap(a.recv_callback_, b.recv_callback_);
        swap(a.server_, b.server_); 
        swap(a.valid_, b.valid_); 
        swap(a.clients_, b.clients_);
        swap(a.pending_connections_, b.pending_connections_);
        swap(a.used_usernames_, b.used_usernames_); 
        swap(a.ID_queue_, b.ID_queue_); 
    }
private:  
    void registerConnection(ENetPeer* peer);
    void sendHandshakeAccepted(const std::size_t ID, const bool accepted);
    void sendHandshakeChallenge(const std::size_t hash); 
    bool verifyHandshakeChallenge(const std::size_t hash, const int result) const; 
private:   
    std::function<void(const PacketData&)> recv_callback_; 
    ENetHost* server_; 
    bool valid_ = false; 
    std::unordered_map<std::size_t, ServerClientInfo> clients_; 
    std::unordered_map<std::size_t, ServerClientInfo> pending_connections_;  
    std::vector<std::string> used_usernames_ = {"server"};
    std::queue<std::size_t> ID_queue_;   
}; 

#endif // SERVER_H 