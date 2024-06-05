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

void ServerEventSystem::init(ECS::Scene &scene)
{
    for (auto&[client, data] : m_NetServer.GetClients())
    {
        auto temp = UserData();
        temp.first = -1; 
        data.user_data = std::move(temp); 
    }

    EventHandler::get().BindEvent(SDL_QUIT, [&](SDL_Event& e) -> void 
    {
        GlobalAppState::get().setAppState(AppState::AS_QUIT);
    });


    for (std::size_t i = 0; i < m_ClientToECS_ID.size(); ++i)
    {
        m_ClientToECS_ID[i] = MakeEntity(scene, Prototype::PLAYER, Vector2(0, 0)); 
        auto& component = scene.getComponent<PhysicsBodyComponent>(m_ClientToECS_ID[i]);
        component.SimulatesPhysics = true;
        scene.setEntityActive(m_ClientToECS_ID[i], false);
    }
}

void ServerEventSystem::SetupServer(const std::uint16_t port)
{
    m_NetServer = NetServer(port, 10, std::bind(&ServerEventSystem::OnRecievePacket, this, std::placeholders::_1));

    if (!m_NetServer.getValid()) 
    {
        GlobalAppState::get().setAppState(AppState::AS_FAIL, "Cannot initialize network!"); 
    }
}


// Most un-ECSy update method ever!!!
void ServerEventSystem::update(ECS::Scene &scene, float delta)
{
    m_CurrentScene = &scene; 

    if (m_NetServer.getValid())
    {
        m_NetServer.updateNetwork();
         
        if (m_NetTickTimer.GetDelta() >= 1.0f / m_NetTickRate) 
        {
            // send an update packet
            // TODO: Wrok on
            PacketData update_packet; 
            ServerUpdatePayload payload;
           
            update_packet.type = PT_GAME_UPDATE; 
            auto& clients = m_NetServer.GetClients();
            payload.clients_size = clients.size(); 
            payload.client_states.resize(payload.clients_size); 

            std::vector<std::size_t> connected_list;
            connected_list.reserve(10); 

            int i = 0;
            for (auto &client : clients)
            {
                connected_list.push_back(client.first); 
                Vector2 client_position;
                auto& component = scene.getComponent<PhysicsBodyComponent>(m_ClientToECS_ID[client.first]);
                client_position = { static_cast<float>(component.BoundingBox.x), static_cast<float>(component.BoundingBox.y) };
                payload.client_states[i] = ClientInfo { static_cast<int>(client.first), client_position }; 
                ++i;
            }

            for (std::size_t i = 0; i < m_ClientToECS_ID.size(); ++i)
            {
                // is it in the connected list? if so, make the entity active again
                auto it = std::find(connected_list.begin(), connected_list.end(), i); 
                if (it == connected_list.end())
                {
                    // make this client dissappear and reset their position 
                    scene.setEntityActive(m_ClientToECS_ID[i], false);
                    auto& component = scene.getComponent<PhysicsBodyComponent>(m_ClientToECS_ID[i]);
                    component = PhysicsBodyComponent { .BoundingBox = component.BoundingBox };
                    component.SimulatesPhysics = true; 
                }
                else 
                {
                    scene.setEntityActive(m_ClientToECS_ID[i]);
                } 
            }

            update_packet.data = payloadToString<ServerUpdatePayload>(payload); 
            update_packet.data_size = update_packet.data.size() + 1; 

            m_NetServer.broadcastPacket(update_packet, 0, false); 
            m_NetTickTimer.Reset();
        }
    } 

    EventHandler::get().Update(); 
}

void ServerEventSystem::OnRecievePacket(const PacketData& packet) 
{
    switch (packet.type) 
    {
    case PT_GAME_UPDATE: 
    {    
        auto payload = payloadFromString<ClientUpdatePayload>(packet.data); 
        ECS::EntityID client = m_ClientToECS_ID[packet.ID];
        auto& component = m_CurrentScene->getComponent<PhysicsBodyComponent>(client);
        //w = 15, a = 14, s = 13, d = 12
        std::max_component_per_entity<16> inputs = payload.input_bits;  
        float x_scale = 0.0f, y_scale = 0.0f;
        std::cout << inputs << std::endl; 

        if (inputs[14]) 
        {
            x_scale = -1; 
        } 
        else if (inputs[13])
        {
            x_scale = 1; 
        }
        else 
        { 
            x_scale = 0;
        }
       
        component.Velocity.X = x_scale * m_PlayerAccelX; 

        std::cout << x_scale << " " << y_scale << std::endl; 
        break; 
    }
    case PT_CLIENT_JUMP: 
    {
        ECS::EntityID client = m_ClientToECS_ID[packet.ID]; 
        auto& component = m_CurrentScene->getComponent<PhysicsBodyComponent>(client);
        component.Velocity.Y = -m_PlayerAccelY; 
        break; 
    }
    }
}

ServerEventSystem::~ServerEventSystem() 
{

}