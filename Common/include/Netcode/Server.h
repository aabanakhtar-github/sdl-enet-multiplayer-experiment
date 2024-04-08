#ifndef SERVER_H 
#define SERVER_H 

#include <cstdint> 
#include <string> 
#include <vector>
#include <string> 
#include <unordered_map>
#include <functional> 

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

#include "enet/enet.h"
#include "Packet.h"
#include "NetUtil.h"

struct ClientData 
{ 
    ENetPeer* Peer; 
    std::string Username;
};

class NetServer
{
    NetServer(const NetServer&) = delete; 
    NetServer& operator = (const NetServer&) = delete; 
public:
    explicit NetServer() : m_Valid(false) {}
    explicit NetServer(const std::uint16_t port, const std::size_t peers, std::function<void(const PacketData&)> recv_callback); 
    ~NetServer(); 
    NetServer(NetServer&&); 
    NetServer& operator = (NetServer&&); 


    void SendPacketTo(const PacketData& packet, const std::size_t client_hash, const int channel, bool reliable = false); 
    void BroadcastPacket(const PacketData& packet, int channel, bool reliable = false); 
    void BroadcastPacketAllExcept(const PacketData& packet, const int channel, const std::size_t client_hash, bool reliable);
    void UpdateNetwork(float block_time = 0.0f); 

    bool GetValid() const { return m_Valid; } 
private:  
    void RegisterConnection(ENetPeer* peer);
private: 
    std::function<void(const PacketData&)> m_RecvCallback; 
    ENetHost* m_Server; 
    bool m_Valid; 
    std::unordered_map<std::size_t, ClientData> m_Clients; 
    std::vector<std::string> m_UsedUsernames = {"server"};  
}; 

#endif // SERVER_H 