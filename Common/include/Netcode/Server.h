#ifndef SERVER_H 
#define SERVER_H 

#include <cstdint> 
#include <string> 
#include <vector>
#include <string> 
#include <unordered_map>
#include <queue>
#include <functional> 

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

#include "enet/enet.h"
#include "Packet.h"
#include "NetUtil.h"

struct ServerClientInfo : ClientInfo 
{
    ENetPeer* Peer = nullptr; 
    int ClientSalt;
    int ServerSalt;  
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

    void SendPacketToPending(const PacketData& packet, const std::size_t hash, const int channel, bool reliable = false);
    void SendPacketTo(const PacketData& packet, const std::size_t ID, const int channel, bool reliable = false); 
    void BroadcastPacket(const PacketData& packet, const int channel, bool reliable = false); 
    void BroadcastPacketAllExcept(const PacketData& packet, const int channel, const std::size_t ID, bool reliable);
    void UpdateNetwork(float block_time = 0.0f); 

    bool GetValid() const { return m_Valid; } 
private:  
    void RegisterConnection(ENetPeer* peer);

    void SendHandshakeAccepted(const std::size_t ID, bool accepted);
    void SendHandshakeChallenge(const std::size_t hash); 
    bool VerifyHandshakeChallenge(const std::size_t hash, const std::uint32_t result) const; 
private: 
    std::function<void(const PacketData&)> m_RecvCallback; 
    ENetHost* m_Server; 
    bool m_Valid; 
    std::unordered_map<std::size_t, ServerClientInfo> m_Clients; 
    std::unordered_map<std::size_t, ServerClientInfo> m_PendingConnections;  
    std::vector<std::string> m_UsedUsernames = {"server"};
    std::queue<std::size_t> m_ID_Queue;   
}; 

#endif // SERVER_H 