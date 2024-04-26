#include "Netcode/Client.h"
#include "AppState.h"
#include <iostream> 

NetClient::NetClient(std::function<void(const PacketData&)> recv_callback)
    : m_RecvCallback(recv_callback), m_Client(nullptr), m_Server{ nullptr, {} }, m_Username(), m_Valid(false), m_Connected(false)
{
    m_Client = enet_host_create(nullptr, 1, 1, 0, 0); 
    if (m_Client == nullptr) 
    {
        GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Cannot initialize network!"); 
        return;  
    }

    m_Valid = true; 
    std::cout << "Created a client successfully!" << std::endl; 
}

NetClient::NetClient(NetClient&& other) 
{
    std::swap(m_RecvCallback, other.m_RecvCallback); 
    std::swap(m_Client, other.m_Client); 
    std::swap(m_Server, other.m_Server); 
    std::swap(m_Username, other.m_Username); 
    std::swap(m_ID, other.m_ID); 
    std::swap(m_Valid, other.m_Valid); 
    std::swap(m_Connected, other.m_Connected); 
    std::swap(m_ClientSalt, other.m_ClientSalt);
    std::swap(m_ServerSalt, other.m_ServerSalt);
}

NetClient& NetClient::operator = (NetClient&& other) 
{
    if (m_Client != nullptr) 
    {
        enet_host_destroy(m_Client); 
    }
    std::swap(m_RecvCallback, other.m_RecvCallback);
    std::swap(m_Client, other.m_Client); 
    std::swap(m_Server, other.m_Server); 
    std::swap(m_Username, other.m_Username); 
    std::swap(m_ID, other.m_ID); 
    std::swap(m_Valid, other.m_Valid); 
    std::swap(m_Connected, other.m_Connected); 
    std::swap(m_ClientSalt, other.m_ClientSalt);
    std::swap(m_ServerSalt, other.m_ServerSalt);
    return *this; 
}

void NetClient::Connect(const std::string& server_addr, const std::uint16_t port, float timeout) 
{
    ENetAddress server_ip; 
    enet_address_set_host_ip(&server_ip, server_addr.c_str()); 
    
    server_ip.port = port; 
    m_Server.Address = server_ip; 
    m_Server.Server = enet_host_connect(m_Client, &m_Server.Address, 1, 0); 
    if (m_Server.Server == nullptr) 
    {
        std::cerr << "Failed to connect to server!" << std::endl; 
        m_Connected = false;  
        return; 
    }

    // check for connection by peeping into the event queue
    ENetEvent event; 
    if (enet_host_service(m_Client, &event, static_cast<int>(timeout * 1000.f)) > 0
        && event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << "Successfully connected!" << std::endl;
    }
    else 
    {
        std::cerr << "Failed to connect to server!" << std::endl; 
        // server is not available... 
        enet_peer_reset(m_Server.Server); 
        m_Connected = false; 
        return;  
    }

    m_Connected = true; 
}

void NetClient::Disconnect(float timeout) 
{
    // connection does not exist ... 
    if (m_Server.Server != nullptr)
    { 
        enet_peer_disconnect(m_Server.Server, 0);
    } 

    m_Connected = false;  
    UpdateNetwork(1.0); 
}

void NetClient::SendPacket(PacketData packet, int channel, bool reliable)
{
    packet.ID = m_ID;
    packet.Salt = m_ClientSalt ^ m_ServerSalt; 
    ENetPacket* netpacket = PacketDataToNetPacket(packet, reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED); 
    enet_peer_send(m_Server.Server, channel, netpacket); 
}

void NetClient::UpdateNetwork(float blocking_time, bool disconnection)
{
    ENetEvent event;
    while (GetConnected() && enet_host_service(m_Client, &event, static_cast<std::uint32_t>(blocking_time * 1000.0f)) > 0) 
    {
        switch (event.type) 
        {
        case ENET_EVENT_TYPE_CONNECT: 
            std::cout << "Connection Succeeded!" << std::endl;   
            break; 

        case ENET_EVENT_TYPE_RECEIVE:
        {
            PacketData parsed_packet = PacketDataFromNetPacket(event.packet); 
                
            switch(parsed_packet.Type)
            {
            case PT_HANDSHAKE:
            {
                std::cout << parsed_packet.Data << std::endl;
                auto payload = PayloadFromString<HandshakeChallengePayload>(parsed_packet.Data); 
                int response = payload.ClientSalt ^ payload.ServerSalt; 
                
                HandshakeResponsePayload response_payload; 
                response_payload.ChallengeResponse = response; 
                
                PacketData response_packet; 
                response_packet.Type = PT_HANDSHAKE_RESULT; 
                response_packet.Data = PayloadToString<HandshakeResponsePayload>(response_payload);  
                response_packet.DataLength = response_packet.Data.size() + 1;
                
                m_ServerSalt = payload.ServerSalt; 
                m_ClientSalt = payload.ClientSalt;

                SendPacket(response_packet, 0, true); 
                break;
            } 
            case PT_HANDSHAKE_RESULT: 
            {
                auto payload = PayloadFromString<HandshakeAcceptRejectPayload>(parsed_packet.Data);
                if (payload.Accepted)
                {
                    m_ID = payload.NewID;
                } 
                else 
                {
                    // reset this client and invalidate state
                    enet_peer_reset(m_Server.Server);
                    m_Connected = false; 
                }
                break; 
            }
            default:    
                if (!disconnection && parsed_packet.Salt == (m_ClientSalt ^ m_ServerSalt)) 
                {
                    m_RecvCallback(parsed_packet); 
                }
                break;
            }
            
            enet_packet_destroy(event.packet); 
            break; 
        }
        case ENET_EVENT_TYPE_DISCONNECT: 
            enet_peer_reset(m_Server.Server);  
            event.peer->data = nullptr;  // enet requires this as the peer is no longer garbage collected :)
            std::cout << "Disconnection Succeeded!" << std::endl;  
            m_Connected = false;  
            break; 
        }
    }
}

NetClient::~NetClient() 
{
    enet_host_destroy(m_Client); 
    m_Client = nullptr; 
}
