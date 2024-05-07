#include "ServerEventSystem.h"
#include "EventHandler.h"
#include "Components.h"
#include "CreateScenes.h"
#include <sstream> 
#include <functional> 

namespace { 
    int up = 0; 
    int down = 0; 
}

using UserData = std::pair<ECS::EntityID, std::array<std::tuple<ClientInfo, ClientUpdatePayload, std::shared_ptr<ECS::EntityID>>, 1024>>;

void ServerEventSystem::Init(ECS::Scene& scene) 
{
    for (auto&[client, data] : m_NetServer.GetClients())
    {
        auto temp = UserData();
        temp.first = -1; 
        data.UserData = std::move(temp); 
    }

    EventHandler::Get().BindEvent(SDL_QUIT, [&](SDL_Event& e) -> void 
    {
        GlobalAppState::Get().SetAppState(AppState::AS_QUIT);
    });


    for (std::size_t i = 0; i < m_ClientToECS_ID.size(); ++i)
    {
        m_ClientToECS_ID[i] = scene.CreateEntity(); 
        BuildPlayer(scene, m_ClientToECS_ID[i]); 
        scene.SetEntityActive(m_ClientToECS_ID[i], false); 
    }
}

void ServerEventSystem::SetupServer(const std::uint16_t port)
{
    m_NetServer = NetServer(port, 10, std::bind(&ServerEventSystem::OnRecievePacket, this, std::placeholders::_1));

    if (!m_NetServer.GetValid()) 
    {
        GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Cannot initialize network!"); 
    }
}


// Most un-ECSy update method ever!!!
void ServerEventSystem::Update(ECS::Scene& scene, float delta)
{
    if (m_NetServer.GetValid())
    {
        m_NetServer.UpdateNetwork();
         
        if (m_NetTickTimer.GetDelta() >= 1.0f / m_NetTickRate) 
        {
            // send an update packet
            // TODO: Wrok on
            PacketData update_packet; 
            ServerUpdatePayload payload;
           
            update_packet.Type = PT_GAME_UPDATE; 
            auto& clients = m_NetServer.GetClients();
            payload.ClientsLength = clients.size(); 
            payload.ClientStates.resize(payload.ClientsLength); 

            std::vector<std::size_t> connected_list;
            connected_list.reserve(10); 

            int i = 0;
            for (auto &client : clients)
            {
                connected_list.push_back(client.first); 
                Vector2 client_position;
                auto& component = scene.GetComponent<PhysicsBodyComponent>(m_ClientToECS_ID[client.first]); 
                client_position = { static_cast<float>(component.BoundingBox.x), static_cast<float>(component.BoundingBox.y) };
                payload.ClientStates[i] = ClientInfo { static_cast<int>(client.first), client_position }; 
                ++i;
            }

            for (std::size_t i = 0; i < m_ClientToECS_ID.size(); ++i)
            {
                // is it in the connected list? if so, make the entity active again
                auto it = std::find(connected_list.begin(), connected_list.end(), i); 
                if (it == connected_list.end())
                {
                    // make this client dissappear and reset their position 
                    scene.SetEntityActive(m_ClientToECS_ID[i], false); 
                    auto& component = scene.GetComponent<PhysicsBodyComponent>(m_ClientToECS_ID[i]);
                    component = PhysicsBodyComponent { .BoundingBox = component.BoundingBox };
                    component.SimulatesPhysics = true; 
                }
                else 
                {
                    scene.SetEntityActive(m_ClientToECS_ID[i]); 
                } 
            }

            update_packet.Data = PayloadToString<ServerUpdatePayload>(payload); 
            update_packet.DataLength = update_packet.Data.size() + 1; 

            m_NetServer.BroadcastPacket(update_packet, 0, false); 
            m_NetTickTimer.Reset();
        }
    } 

    EventHandler::Get().Update(); 
}

void ServerEventSystem::OnRecievePacket(const PacketData& packet) 
{
}

ServerEventSystem::~ServerEventSystem() 
{

}