#include "Netcode/Server.h" 
#include "Netcode/NetUtil.h"
#include "AppState.h"
#include <algorithm>

NetServer::NetServer(const std::uint16_t port, const std::size_t peers, std::function<void(const PacketData&)> recv_callback) 
    : m_RecvCallback(recv_callback), m_Server(nullptr), m_Valid(false), m_Clients()
{
    ENetAddress addr;
    addr.host = ENET_HOST_ANY; 
    addr.port = port;
    
    m_Server = enet_host_create(&addr, peers, 1, 0, 0); 
    if (m_Server == nullptr) 
    {
        GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Cannot initialize network!");         
        std::cerr << "Couldn't intialize server!" << std::endl;
        m_Valid = false;  
        return;
    }

    m_Valid = true; 
    char ip_buffer[128] = { 0 };     
    enet_address_get_host_ip(&addr, ip_buffer, 128); 

    std::cout << "Server created successfully on ip: " << ip_buffer <<  " and on port " << port << "!" << std::endl; 
}

NetServer::NetServer(NetServer&& other)
{
    std::swap(m_RecvCallback, other.m_RecvCallback); 
    std::swap(m_Server, other.m_Server); 
    std::swap(m_Valid, other.m_Valid); 
    std::swap(m_Clients, other.m_Clients); 
    std::swap(m_UsedUsernames, other.m_UsedUsernames); 
}

NetServer& NetServer::operator = (NetServer&& other)
{
    std::swap(m_RecvCallback, other.m_RecvCallback); 
    std::swap(m_Server, other.m_Server); 
    std::swap(m_Valid, other.m_Valid); 
    std::swap(m_Clients, other.m_Clients); 
    std::swap(m_UsedUsernames, other.m_UsedUsernames);   
    return *this; 
}

NetServer::~NetServer()
{
    enet_host_destroy(m_Server); 
    m_Server = nullptr; 
}

void NetServer::BroadcastPacketAllExcept(const PacketData& packet, const int channel, const std::size_t client_hash, bool reliable) 
{
    ENetPacket* net_packet = PacketDataToNetPacket(packet, reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED); 
    for (auto&[client, peer] : m_Clients) 
    {
        if (client_hash != client) 
        {
            enet_peer_send(peer.Peer, channel, net_packet); 
        }
    }
}

void NetServer::RegisterConnection(ENetPeer* peer) 
{
    ClientData peer_data; 
    peer_data.Peer = peer; 
    peer_data.Username = "unverified"; 
    m_Clients[std::hash<ENetAddress>()(peer->address)] = peer_data;
}

void NetServer::SendPacketTo(const PacketData& packet, const std::size_t client_hash, const int channel, bool reliable)
{
    if (m_Clients.contains(client_hash))
    {
        ENetPacket* net_packet = PacketDataToNetPacket(packet, reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED);
        enet_peer_send(m_Clients[client_hash].Peer, channel, net_packet); 
    }
}

void NetServer::UpdateNetwork(float block_time) 
{
   ENetEvent event; 
   while (enet_host_service(m_Server, &event, static_cast<int>(block_time * 1000.0f) > 0))
   {
        switch (event.type) 
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
            char ip_buffer[128] = { 0 }; 
            enet_address_get_host_ip(&event.peer->address, ip_buffer, sizeof(ip_buffer));
            std::cout << "Recieved a new connection from client: " << ip_buffer << "!" << std::endl; 
            RegisterConnection(event.peer); 
            break;
        } 
        case ENET_EVENT_TYPE_RECEIVE:
        {   
            PacketData parsed_packet = PacketDataFromNetPacket(event.packet); 
            m_RecvCallback(parsed_packet); 
            enet_packet_destroy(event.packet); 
            break;  
        }
        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "A client disconnected!" << std::endl; 
            event.peer->data = nullptr; 
            enet_peer_reset(event.peer);
            break; 
        }
   }
}

void NetServer::BroadcastPacket(const PacketData& packet, int channel, bool reliable)
{
    ENetPacket* net_packet = PacketDataToNetPacket(packet, reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED); 
    enet_host_broadcast(m_Server, channel, net_packet); 
}