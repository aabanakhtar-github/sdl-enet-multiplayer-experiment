#include "Netcode/Server.h" 
#include "Netcode/NetUtil.h"
#include "AppState.h"
#include <algorithm>
#include <numeric> 

NetServer::NetServer(const std::uint16_t port, const std::size_t peers, std::function<void(const PacketData&)> recv_callback) 
    : m_RecvCallback(recv_callback), m_Server(nullptr), m_Valid(false), m_Clients(), m_ID_Queue()
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

    for (std::size_t i = 0; i < peers; ++i) 
    {
        m_ID_Queue.push(i);
    }
}

NetServer::NetServer(NetServer&& other)
{
    std::swap(m_RecvCallback, other.m_RecvCallback); 
    std::swap(m_Server, other.m_Server); 
    std::swap(m_Valid, other.m_Valid); 
    std::swap(m_Clients, other.m_Clients); 
    std::swap(m_UsedUsernames, other.m_UsedUsernames); 
    std::swap(m_ID_Queue, other.m_ID_Queue);  
}

NetServer& NetServer::operator = (NetServer&& other)
{
    std::swap(m_RecvCallback, other.m_RecvCallback); 
    std::swap(m_Server, other.m_Server); 
    std::swap(m_Valid, other.m_Valid); 
    std::swap(m_Clients, other.m_Clients); 
    std::swap(m_UsedUsernames, other.m_UsedUsernames);
    std::swap(m_ID_Queue, other.m_ID_Queue);  
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
    ServerClientInfo peer_data; 
    peer_data.Peer = peer;  
    peer_data.Hash = std::hash<ENetAddress>()(peer->address); 
    m_PendingConnections[std::hash<ENetAddress>()(peer->address)] = peer_data;
    SendHandshakeChallenge(peer_data.Hash);
}

void NetServer::SendHandshakeChallenge(const std::size_t hash)
{
    auto it = m_PendingConnections.find(hash); 
    if (it != m_PendingConnections.end())
    {
        // windows api issue where they used MACROs
        constexpr int high = (std::numeric_limits<int>::max)();
        HandshakeChallengePayload payload;  
        int server_salt = Random(1, high); 
        int client_salt = Random(1, high);  
        it->second.ServerSalt = server_salt; 
        it->second.ClientSalt = client_salt; 
        payload.ClientSalt = client_salt; 
        payload.ServerSalt = server_salt; 

        PacketData packet; 
        packet.ID = 0; 
        packet.Type = PT_HANDSHAKE; 
        packet.Data = PayloadToString<HandshakeChallengePayload>(payload);  
        packet.DataLength = packet.Data.length() + 1;   
        std::cout << "Sending challenge " << server_salt << " " << client_salt << " "  << (server_salt ^ client_salt) << std::endl; 
        SendPacketToPending(packet, hash, 0, true); 
    }
}

bool NetServer::VerifyHandshakeChallenge(const std::size_t hash, const int result) const
{
    auto it = m_PendingConnections.find(hash); 
    if (it != m_PendingConnections.end())
    {
        int real_result = it->second.ServerSalt ^ it->second.ClientSalt;
        if (real_result == result)
        {
            return true;
        }
    } 
    return false; 
}

void NetServer::SendPacketToPending(const PacketData& packet, const std::size_t hash, const int channel, bool reliable)
{
    auto it = m_PendingConnections.find(hash); 
    if (it != m_PendingConnections.end())
    {
        ENetPacket* net_packet = PacketDataToNetPacket(packet, reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT); 
        enet_peer_send(it->second.Peer, channel, net_packet);  
    }  
}

void NetServer::SendPacketTo(const PacketData& packet, const std::size_t ID, const int channel, bool reliable)
{
    auto it = m_Clients.find(ID); 
    if (it != m_Clients.end())
    {
        ENetPacket* net_packet = PacketDataToNetPacket(packet, reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);  
        enet_peer_send(it->second.Peer, channel, net_packet); 
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

            switch (parsed_packet.Type)
            {
            case PT_HANDSHAKE_RESULT:
            {
                auto payload = PayloadFromString<HandshakeResponsePayload>(parsed_packet.Data); 
                if (VerifyHandshakeChallenge(std::hash<ENetAddress>()(event.peer->address), payload.ChallengeResponse))
                {
                    ENetAddress this_address = event.peer->address; 
                    std::size_t client_hash = std::hash<ENetAddress>()(this_address); 

                    auto it = m_PendingConnections.find(client_hash); 
                    if (it == m_PendingConnections.end())
                    {    
                        break; 
                    }
 
                    std::size_t client_ID = m_ID_Queue.front();
                    m_ID_Queue.pop(); 
                    
                    event.peer->data = new std::size_t(client_ID);

                    ServerClientInfo client_info; 
                    client_info.ID = client_ID;
                    client_info.ClientSalt = it->second.ClientSalt; 
                    client_info.ServerSalt = it->second.ServerSalt; 
                    client_info.Peer = event.peer;
                    client_info.Hash = client_hash; 

                    m_Clients.emplace(client_ID, std::move(client_info));
                    m_PendingConnections.erase(it);  
                    SendHandshakeAccepted(client_ID, true); 
                }
                else 
                {
                    ENetAddress this_address = event.peer->address; 
                    std::size_t hash = std::hash<ENetAddress>()(this_address); 
                    
                    auto it = m_PendingConnections.find(hash);  
                    if (it != m_PendingConnections.end())
                    {
                        enet_peer_reset(it->second.Peer);
                        m_PendingConnections.erase(it);
                    }

                    SendHandshakeAccepted(-1, false); 
                }
                break;
            }
            default: 
                m_RecvCallback(parsed_packet); 
                break;    
            } 
            enet_packet_destroy(event.packet); 
            break;  
        }
        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "A client disconnected!" << std::endl; 
            m_ID_Queue.push(*((std::size_t*)event.peer->data)); 
            delete (std::size_t*)event.peer->data; 
            event.peer->data = nullptr;              
            
            enet_peer_reset(event.peer);
            break; 
        }
   }
}

void NetServer::SendHandshakeAccepted(const std::size_t ID, const bool accepted)
{
    HandshakeAcceptRejectPayload payload;
    PacketData packet; 

    payload.Accepted = accepted;
    payload.NewID = ID; 

    packet.Type = PT_HANDSHAKE_RESULT;
    packet.Data = PayloadToString<HandshakeAcceptRejectPayload>(payload); 
    packet.DataLength = packet.Data.size() + 1; 

    SendPacketTo(packet, 0, true);  
}

void NetServer::BroadcastPacket(const PacketData& packet, int channel, bool reliable)
{
    ENetPacket* net_packet = PacketDataToNetPacket(packet, reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED); 
    enet_host_broadcast(m_Server, channel, net_packet); 
}