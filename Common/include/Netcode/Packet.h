#ifndef PACKET_H 
#define PACKET_H 

#include <cstdint> 
#include <string> 
#include <istream> 
#include "MathFuncs.h"

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

#include "enet/enet.h" 

enum PacketType 
{ 
    PT_GAME_UPDATE, 
    PT_CONNECT_UPDATE, 
    PT_DISCONNECT_UPDATE,
    PT_HANDSHAKE, 
    PT_HANDSHAKE_RESULT
};

struct ClientInfo
{
    std::string Username; 
    std::uint32_t ID = -1; 
    std::uint32_t LastProcessedInputID; 
    Vector2 Position;
}; 

struct PacketData
{
    std::uint8_t ProtocolVersion = (1 << 4) | (1); 
    std::uint32_t Checksum; 
    std::uint32_t ID;
    PacketType Type; 
    std::uint32_t DataLength;
    std::string Data; 
};

// Client side only
struct ConnectDisconnectPayload 
{
    std::uint32_t ID; 
    std::uint32_t UsernameLength; 
    std::string UsernameRequest; 
}; 

struct HandshakeChallengePayload 
{
    std::uint32_t ServerSalt; 
    std::uint32_t ClientSalt; 
}; 

// Client side only
struct HandshakeResponsePayload 
{
    std::uint32_t ChallengeResponse; 
}; 

struct HandshakeAcceptRejectPayload 
{
    std::uint8_t Accepted; 
    std::uint32_t NewID; 
}; 

struct ClientUpdatePayload
{
    std::uint64_t Salt; 
    std::uint64_t SequenceNumber; 
    Vector2 Input; 
};

struct ServerUpdatePayload 
{
    std::uint32_t Salt; 
    std::uint64_t SequenceNumber; 
    std::uint32_t ClientsLength; 
    std::vector<ClientInfo> ClientStates; 
}; 

template<typename T> 
inline std::string PayloadToString(const T& payload); 

template<typename T> 
inline std::string PayloadString(const T& payload); 

inline std::istream& operator >> (std::istream& in, PacketType& type) 
{
    int val; 
    in >> val; 
    type = static_cast<PacketType>(val);
    return in; 
}
ENetPacket* PacketDataToNetPacket(const PacketData& packet, ENetPacketFlag flags); 
PacketData PacketDataFromNetPacket(const ENetPacket* packet); 

#endif // PACKET_H