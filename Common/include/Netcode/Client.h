#ifndef CLIENT_H 
#define CLIENT_H 

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#include "enet/enet.h"
#include "Packet.h"
#include <functional>

struct ServerData
{
    ENetPeer* Server; 
    ENetAddress Address; 
};

class NetClient 
{
    NetClient(const NetClient&) = delete; 
    NetClient& operator = (const NetClient&) = delete; 
public: 
    explicit NetClient() : m_Valid(false) {} 
    explicit NetClient(std::function<void(const PacketData&)> recv_callback); 
    NetClient(NetClient&&);
    NetClient& operator = (NetClient&&); 
    ~NetClient(); 

    void UpdateNetwork(float block_time = 0.0, bool disconnection = false);
    void Connect(const std::string& server_addr, const std::uint16_t port, float timeout); 
    void Disconnect(float timeout);
    void SendPacket(PacketData packet, int channel, bool reliable = false); 

    bool GetConnected() const { return m_Valid && m_Connected; } 
    bool GetValid() const { return m_Valid; }
private: 
    std::function<void(const PacketData&)> m_RecvCallback; 
    ENetHost* m_Client; 
    ServerData m_Server; 
    std::string m_Username;
    bool m_Valid; 
    bool m_Connected; 
    int m_ID = -1;  
    int m_ServerSalt = -1; 
    int m_ClientSalt = -1; 
};


#endif 