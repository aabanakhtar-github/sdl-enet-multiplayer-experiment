#include "Netcode/Server.h"
#include <thread> 
#include <iostream> 

void recv_callback(const PacketData& packet)
{
    std::cout << "Recieved packet: " << packet.Data << std::endl; 
}

int main(int argc, char* argv[]) 
{
    enet_initialize(); 

    NetServer* game_client = new NetServer(7777, 10, recv_callback);

    std::thread t([&]() -> void 
    {
        while (game_client->GetValid())
            game_client->UpdateNetwork(0.01); 
    }); 

    while(game_client->GetValid())
    {
        std::string input; 
        std::getline(std::cin, input); 

        PacketData p = { 
            .ID = 0,
            .Type = PACKET_TYPE_GAME_UPDATE, 
            .DataLength = 0, 
            .Data = input
        }; 

        if (input == "Shutdown!") 
        {
            break; 
        }

        game_client->BroadcastPacket(p, 0, true); 
    }

    t.join();
    delete game_client; 
    enet_deinitialize();  
    return 0;
}