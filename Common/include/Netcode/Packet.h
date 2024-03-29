#ifndef PACKET_H 
#define PACKET_H 

#include <cstdint> 
#include <string> 
#include <istream> 

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif 

#include "enet/enet.h" 

enum PacketType 
{ 
    PACKET_TYPE_GAME_UPDATE, 
    PACKET_TYPE_DISCONNECT,
    PACKET_TYPE_CONNECT
};

struct PacketData
{
    std::uint32_t ID;
    PacketType Type; 
    std::uint32_t DataLength;
    std::string Data; 
};

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