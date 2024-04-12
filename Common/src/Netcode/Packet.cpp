#include "Netcode/Packet.h"
#include <sstream> 
#include <iostream> 

template<> 
HandshakeChallengePayload PayloadFromString<HandshakeChallengePayload>(const std::string& data)
{
    HandshakeChallengePayload return_value{ }; 
    std::istringstream ss(data); 

    ss >> return_value.ServerSalt; 
    ss >> return_value.ClientSalt;
    return return_value; 
}

template<> 
std::string PayloadToString<HandshakeChallengePayload>(const HandshakeChallengePayload& data)
{
    std::ostringstream ss; 
    ss << data.ServerSalt << " "; 
    ss << data.ClientSalt << " "; 
    return ss.str(); 
}

template<>
HandshakeResponsePayload PayloadFromString<HandshakeResponsePayload>(const std::string& data)
{
    HandshakeResponsePayload return_value;
    std::string data_copy = data;

    std::erase_if(data_copy, isspace); 
    return_value.ChallengeResponse = std::stoi(data_copy);
    
    return return_value;
}

template<> 
std::string PayloadToString<HandshakeResponsePayload>(const HandshakeResponsePayload& data) 
{
    return std::to_string(data.ChallengeResponse); 
}

std::istream& operator >> (std::istream& in, PacketType& type)
{
    int val; 
    in >> val; 
    type = static_cast<PacketType>(val);
    return in; 
}

ENetPacket* PacketDataToNetPacket(const PacketData& packet, ENetPacketFlag flags)
{
    std::stringstream ss(std::ios_base::out);
    ss << packet.ID << " "; 
    ss << packet.Type << " "; 
    ss << packet.DataLength << " ";  
    ss << packet.Data; 
    std::string data = ss.str();
    return enet_packet_create(data.c_str(), data.size() + 1, flags);  
}

PacketData PacketDataFromNetPacket(const ENetPacket* packet) 
{
    if (packet == nullptr) 
    {
        std::cerr << "Given a null packet to process in PacketDataFromNetPacket. " << std::endl; 
        return {}; 
    }

    PacketData return_value{};  
    std::string data((char*)packet->data, packet->dataLength); 
    std::istringstream ss(data); 

    ss >> return_value.ID; 
    ss >> return_value.Type; 
    ss >> return_value.DataLength;
    ss.get();  
    std::getline(ss, return_value.Data, static_cast<char>(0)); 

    return return_value; 
}
