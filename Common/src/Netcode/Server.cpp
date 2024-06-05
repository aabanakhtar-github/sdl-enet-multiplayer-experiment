#include "Netcode/Server.h" 
#include "Netcode/NetUtil.h"
#include "Util.h"
#include <iostream>
#include <algorithm>
#include <numeric> 

NetServer::NetServer(const std::uint16_t port, const std::size_t peers, std::function<void(const PacketData&)> recv_callback) 
    : recv_callback_(recv_callback),
      server_(nullptr) {
    ENetAddress addr;
    addr.host = ENET_HOST_ANY;
    addr.port = port;
    
    server_ = enet_host_create(&addr, peers, 1, 0, 0); 

    if (server_ == nullptr) {
        GlobalAppState::get().setAppState(AppState::AS_FAIL, "Cannot initialize network!");         
        std::cerr << "Couldn't intialize server!" << std::endl;
        valid_ = false;  
        return;
    }

    valid_ = true; 
    char ip_buffer[128] = { 0 };     
    enet_address_get_host_ip(&addr, ip_buffer, 128); 

    std::cout << "Server created successfully on ip: " << ip_buffer <<  " and on port " << port << "!" << std::endl; 

    for (std::size_t i = 0; i < peers; ++i) {
        ID_queue_.push(i);
    }
}

NetServer::NetServer(NetServer&& other) {
    swap(*this, other);
}

NetServer& NetServer::operator = (NetServer&& other) {
    swap(*this, other);
    return *this; 
}

NetServer::~NetServer()
{
    if (server_ != nullptr) {
        enet_host_destroy(server_); 
    }

    server_ = nullptr; 
}

void NetServer::registerConnection(ENetPeer* peer) {
    ServerClientInfo peer_data; 
    peer_data.peer = peer;  
    peer_data.hash = std::hash<ENetAddress>()(peer->address); 
    pending_connections_[std::hash<ENetAddress>()(peer->address)] = peer_data;
    sendHandshakeChallenge(peer_data.hash);
}

void NetServer::sendHandshakeChallenge(const std::size_t hash) {
    auto it = pending_connections_.find(hash); 
   
    if (it != pending_connections_.end()) {
        // windows api issue where they used gosh darn macros for "m"
        constexpr int high = (std::numeric_limits<int>::max)();
        HandshakeChallengePayload payload;  
        int server_salt = Random(1, high); 
        int client_salt = Random(1, high);  
        it->second.server_salt = server_salt; 
        it->second.client_salt = client_salt; 
        payload.client_salt = client_salt; 
        payload.server_salt = server_salt; 

        PacketData packet; 
        packet.ID = 0; 
        packet.type = PT_HANDSHAKE; 
        packet.data = payloadToString<HandshakeChallengePayload>(payload);  
        packet.data_size = packet.data.length() + 1;   
        std::cout << "Sending challenge " << server_salt << " " << client_salt << " "  << (server_salt ^ client_salt) << std::endl; 
        sendPacketToPending(packet, hash, 0, true); 
    }
}

bool NetServer::verifyHandshakeChallenge(const std::size_t hash, const int result) const {
    auto it = pending_connections_.find(hash); 
    
    if (it != pending_connections_.end()) {
        int real_result = it->second.server_salt ^ it->second.client_salt;
        
        if (real_result == result) {
            return true;
        }
    } 

    return false; 
}

void NetServer::sendPacketToPending(const PacketData& packet, const std::size_t hash, const int channel, bool reliable) {
    auto it = pending_connections_.find(hash); 
    
    if (it != pending_connections_.end()) {
        ENetPacket* net_packet = packetDataToEnetPacket(packet, reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT); 
        
        if (it->second.peer != nullptr) {
            enet_peer_send(it->second.peer, channel, net_packet); 
        } 
    }  
}

void NetServer::sendPacketTo(PacketData& packet, const std::size_t ID, const int channel, bool reliable) {
    auto it = clients_.find(ID); 

    if (it != clients_.end()) {
        packet.salt = it->second.server_salt ^ it->second.client_salt;
        ENetPacket* net_packet = packetDataToEnetPacket(packet, reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);   
        
        if (it->second.peer != nullptr) {
            enet_peer_send(it->second.peer, channel, net_packet); 
        } 
    }
}

void NetServer::broadcastPacket(PacketData& packet, int channel, bool reliable) {
    for (auto&[ID, client] : clients_) {
        packet.salt = client.server_salt ^ client.client_salt; 
        ENetPacket* net_packet = packetDataToEnetPacket(packet, reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT); 
        
        if (client.peer != nullptr) { 
            enet_peer_send(client.peer, channel, net_packet);         
        }
    }
}

void NetServer::broadcastPacketToAllExcept(PacketData& packet, const int channel, const std::size_t ID, bool reliable) {
    for (auto&[client, peer] : clients_) {
        if (ID != client) {
            packet.salt = peer.client_salt ^ peer.server_salt; 
            ENetPacket* net_packet = packetDataToEnetPacket(packet, reliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNSEQUENCED); 
            
            if (peer.peer != nullptr) { 
                enet_peer_send(peer.peer, channel, net_packet); 
            } 
        }
    }
}

void NetServer::updateNetwork(float block_time) {
    if (server_ == nullptr) { 
        return;
    }

    ENetEvent event; 
   
    while (enet_host_service(server_, &event, static_cast<int>(block_time * 1000.0f) > 0)) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            char ip_buffer[128] = { 0 }; 
            event.peer->data = new int(-1);   
            
            enet_address_get_host_ip(&event.peer->address, ip_buffer, sizeof(ip_buffer));
            std::cout << "Recieved a new connection from client: " << ip_buffer << "!" << std::endl; 
            registerConnection(event.peer); 
            break;
        } 
        case ENET_EVENT_TYPE_RECEIVE: {   
            PacketData parsed_packet = packetDataFromEnetPacket(event.packet); 

            switch (parsed_packet.type) {
            case PT_HANDSHAKE_RESULT: {
                auto payload = payloadFromString<HandshakeResponsePayload>(parsed_packet.data); 
                
                if (verifyHandshakeChallenge(std::hash<ENetAddress>()(event.peer->address), payload.response)) {
                    ENetAddress this_address = event.peer->address; 
                    std::size_t client_hash = std::hash<ENetAddress>()(this_address); 
                    auto it = pending_connections_.find(client_hash); 
                    
                    if (it == pending_connections_.end()) {    
                        break; 
                    }

                    std::size_t client_ID = ID_queue_.front();
                    ID_queue_.pop(); 
                
                    *static_cast<int*>(event.peer->data) = client_ID;

                    ServerClientInfo client_info; 
                    client_info.ID = client_ID;
                    client_info.client_salt = it->second.client_salt; 
                    client_info.server_salt = it->second.server_salt; 
                    client_info.peer = event.peer;
                    client_info.hash = client_hash; 

                    clients_.emplace(client_ID, std::move(client_info));
                    pending_connections_.erase(it);  
                    sendHandshakeAccepted(client_ID, true);
                } else {
                    ENetAddress this_address = event.peer->address; 
                    std::size_t hash = std::hash<ENetAddress>()(this_address); 
                    auto it = pending_connections_.find(hash);  
                    
                    if (it != pending_connections_.end()) {
                        enet_peer_reset(it->second.peer);
                        pending_connections_.erase(it);
                    }

                    sendHandshakeAccepted(-1, false); 
                }
           
                break;
            }
            default: {
                auto it = clients_.find(parsed_packet.ID);
                
                if (it != clients_.end() && (it->second.client_salt ^ it->second.server_salt) == parsed_packet.salt) {
                    recv_callback_(parsed_packet); 
                }
          
                break;    
            } 
            } 
         
            enet_packet_destroy(event.packet); 
            break;  
        }
        case ENET_EVENT_TYPE_DISCONNECT: {
            std::cout << "A client disconnected!" << std::endl; 
            auto ID = *static_cast<int*>(event.peer->data); 
        
            if (event.peer->data != nullptr && ID != -1) {
                ID_queue_.push(*static_cast<std::size_t*>(event.peer->data)); 
                clients_.erase(ID);
            }
            
            delete static_cast<std::size_t*>(event.peer->data); 
            event.peer->data = nullptr;              
            enet_peer_reset(event.peer);
            break; 
        }
        }
    }
   
}

void NetServer::sendHandshakeAccepted(const std::size_t ID, const bool accepted) {
    HandshakeAcceptRejectPayload payload;
    PacketData packet; 

    payload.accepted = accepted;
    payload.new_ID = ID; 

    packet.type = PT_HANDSHAKE_RESULT;
    packet.data = payloadToString<HandshakeAcceptRejectPayload>(payload); 
    packet.data_size = packet.data.size() + 1; 

    std::cout << "Establishing connection with Client, ID: " << payload.new_ID << std::endl; 
    sendPacketTo(packet, ID, 0, true); 
}
