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
    PT_HANDSHAKE_RESULT,
    PT_INVALID
};

struct ClientInfo
{
    int ID = -1; 
    std::size_t Hash; 
    int LastProcessedInputID = 0; 
    Vector2 Position = { 0, 0 };
}; 

#define MAKE_VERSION(a, b) (a << 4) | (b)

struct PacketData
{
    std::int8_t ProtocolVersion = MAKE_VERSION(1, 0); 
    int Checksum = -1; 
    int ID = -1;
    PacketType Type = PT_INVALID; 
    int DataLength = 0;
    std::string Data; 
};

// Client side only
struct ConnectDisconnectPayload 
{
    int ID = -1; 
    int UsernameLength = 0; 
    std::string UsernameRequest; 
}; 

struct HandshakeChallengePayload 
{
    int ServerSalt = 0; 
    int ClientSalt = 0; 
}; 

// Client side only
struct HandshakeResponsePayload 
{
    int ChallengeResponse = 0; 
}; 

struct HandshakeAcceptRejectPayload 
{
    bool Accepted = false; 
    int NewID = -1; 
}; 

struct ClientUpdatePayload
{
    int Salt; 
    int SequenceNumber; 
    Vector2 Input; 
};

struct ServerUpdatePayload 
{
    int Salt; 
    std::int64_t SequenceNumber = -1; 
    int ClientsLength = 0; 
    std::vector<ClientInfo> ClientStates; 
}; 

template<typename T> 
std::string PayloadToString(const T& payload); 
template<typename T> 
T PayloadFromString(const std::string& payload); 

std::istream& operator >> (std::istream& in, PacketType& type) ;
ENetPacket* PacketDataToNetPacket(const PacketData& packet, ENetPacketFlag flags); 
PacketData PacketDataFromNetPacket(const ENetPacket* packet); 

#endif // PACKET_H


