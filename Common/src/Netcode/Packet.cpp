#include "Netcode/Packet.h"
#include <sstream> 
#include <iostream> 

template<> 
HandshakeChallengePayload payloadFromString<HandshakeChallengePayload>(const std::string& data)
{
    HandshakeChallengePayload return_value{ }; 
    std::istringstream ss(data); 
    ss >> return_value.server_salt; 
    ss >> return_value.client_salt;
    return return_value; 
}

template<> 
std::string payloadToString<HandshakeChallengePayload>(const HandshakeChallengePayload& data)
{
    std::ostringstream ss; 
    ss << data.server_salt << " "; 
    ss << data.client_salt << " "; 
    return ss.str(); 
}

template<>
HandshakeResponsePayload payloadFromString<HandshakeResponsePayload>(const std::string& data)
{
    HandshakeResponsePayload return_value;
    std::string data_copy = data;
    std::erase_if(data_copy, isspace); 
    
    return_value.response = std::stoi(data_copy);
    return return_value;
}

template<> 
std::string payloadToString<HandshakeResponsePayload>(const HandshakeResponsePayload& data) 
{
    return std::to_string(data.response); 
}

template<> 
HandshakeAcceptRejectPayload payloadFromString<HandshakeAcceptRejectPayload>(const std::string& data)
{
    HandshakeAcceptRejectPayload return_value;
    std::istringstream ss(data); 
    ss >> return_value.accepted; 
    ss >> return_value.new_ID; 
    return return_value; 
}

template<> 
std::string payloadToString<HandshakeAcceptRejectPayload>(const HandshakeAcceptRejectPayload& data)
{
    std::ostringstream ss; 
    ss << data.accepted << " ";
    ss << data.new_ID << " "; 
    
    return ss.str();  
}

template<> 
ServerUpdatePayload payloadFromString<ServerUpdatePayload>(const std::string& data) 
{
    ServerUpdatePayload return_value; 
    std::istringstream ss(data); 
    ss >> return_value.clients_size; 
    
    for (int i = 0; i < return_value.clients_size; ++i)
    {
        ClientInfo client; 
        ss >> client.ID >> client.position.X >> client.position.Y; 
        return_value.client_states.emplace_back(std::move(client));
    }

    return return_value;
}

template<>
std::string payloadToString<ServerUpdatePayload>(const ServerUpdatePayload& payload)
{
    std::ostringstream ss; 
    ss << payload.clients_size << " "; 
    for (auto& client : payload.client_states)
    {
        ss << client.ID << " " << client.position.X << " " << client.position.Y << " ";
    } 

    return ss.str();
}

template<> 
std::string payloadToString<ClientUpdatePayload>(const ClientUpdatePayload& payload) 
{
    std::ostringstream ss; 
    ss << payload.input_bits << " "; 

    return ss.str();
}

template<> 
ClientUpdatePayload payloadFromString<ClientUpdatePayload>(const std::string& data) 
{
    ClientUpdatePayload return_value; 
    std::istringstream ss(data);
    ss >> return_value.input_bits; 
    return return_value;  
}

std::istream& operator >> (std::istream& in, PacketType& type)
{
    int val; 
    in >> val; 
    type = static_cast<PacketType>(val);
    return in; 
}


ENetPacket* packetDataToEnetPacket(const PacketData& packet, ENetPacketFlag flags)
{
    std::stringstream ss(std::ios_base::out);
    ss << packet.ID << " ";
    ss << packet.salt << " ";  
    ss << packet.type << " "; 
    ss << packet.data_size << " ";  
    ss << packet.data; 
    std::string data = ss.str();
    return enet_packet_create(data.c_str(), data.size() + 1, flags);  
}

PacketData packetDataFromEnetPacket(const ENetPacket* packet) 
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
    ss >> return_value.salt;
    ss >> return_value.type; 
    ss >> return_value.data_size;
    ss.get();  
    std::getline(ss, return_value.data, static_cast<char>(0)); 

    return return_value; 
}
