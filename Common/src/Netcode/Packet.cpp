#include "Netcode/Packet.h"
#include <sstream> 
#include <iostream> 

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
    std::istringstream data_stream(data); 

    data_stream >> return_value.ID; 
    data_stream >> return_value.Type; 
    data_stream >> return_value.DataLength;
    data_stream.get();  
    std::getline(data_stream, return_value.Data, static_cast<char>(0)); 
    /*
    data_stream.get(); 

    char* buffer = new char[return_value.DataLength]; 
    for (std::size_t i = 0; i < return_value.DataLength; ++i)
    {
        buffer[i] = data_stream.get(); 
    }

    buffer[return_value.DataLength-1] = 0;
    return_value.Data = std::string(buffer); 
    delete[] buffer; 
    */

    return return_value; 
}