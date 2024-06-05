#ifndef PACKET_H 
#define PACKET_H 

#include <cstdint> 
#include <string> 
#include <array> 
#include <istream> 
#include "MathFuncs.h"

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

#include "enet/enet.h" 

enum PacketType { 
    PT_GAME_UPDATE, 
    PT_CONNECT_UPDATE, 
    PT_DISCONNECT_UPDATE,
    PT_HANDSHAKE, 
    PT_HANDSHAKE_RESULT,
    PT_CLIENT_JOINED_OR_LEFT,
    PT_CLIENT_JUMP, 
    PT_INVALID
};

struct ClientInfo {
    int ID = -1; 
    Vector2 position = { 0, 0 }; 
}; 

struct PacketData {
    int salt = 0;
    int ID = -1;
    PacketType type = PT_INVALID; 
    std::size_t data_size = 0;
    std::string data; 
};

// Client side only
struct ConnectDisconnectPayload {
    int ID = -1; 
    std::size_t username_size = 0; 
    std::string username_request; 
}; 

struct HandshakeChallengePayload {
    int server_salt = 0; 
    int client_salt = 0; 
}; 

// Client side only
struct HandshakeResponsePayload {
    int response = 0; 
}; 

struct HandshakeAcceptRejectPayload {
    bool accepted = false; 
    int new_ID = -1; 
}; 

struct ClientUpdatePayload {
    std::uint16_t input_bits = 0; 
};

struct ServerUpdatePayload {
    std::size_t clients_size = 0; 
    std::vector<ClientInfo> client_states; 
}; 

template<typename T> 
std::string payloadToString(const T& payload); 
template<typename T> 
T payloadFromString(const std::string& payload); 

std::istream& operator >> (std::istream& in, PacketType& type) ;
ENetPacket* packetDataToEnetPacket(const PacketData& packet, ENetPacketFlag flags); 
PacketData packetDataFromEnetPacket(const ENetPacket* packet); 

#endif // PACKET_H


