#include "ClientEventSystem.h"
#include "Components.h"
#include <cstdint>
#include <sstream> 
#include <iostream>
#include "CreateScenes.h"

// TODO left: 
// - Client server view synchronization (Clients see same as server) / correction
// - Client inputs sent to server
// - Server response to client input
// - Interpolation (depends)

void ClientEventSystem::Init(ECS::Scene& scene) 
{
    m_NetClient = NetClient(std::bind(&ClientEventSystem::OnRecievePacket, this, std::placeholders::_1)); 
    if (!m_NetClient.GetValid()) 
    {
        GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Cannot initialize network!"); 
    }

// TODO refactor into reusable function that allows game to persist across multiple servers
    m_NetClient.Connect("127.0.0.1", 7777, 3.0); 

    EventHandler::Get().BindEvent(SDL_KEYDOWN, [&](SDL_Event& e) -> void
    { 
        switch (e.key.keysym.sym) 
        {
            case SDLK_SPACE:
                break;
        }  
    });   

    EventHandler::Get().BindEvent(SDL_QUIT, [&](SDL_Event& e) -> void 
    {
        GlobalAppState::Get().SetAppState(AppState::AS_QUIT);
    });

    // Create Players 
    for (std::size_t i = 0; i < m_OtherPeers.size(); ++i)
    {
        m_OtherPeers[i].ID = scene.CreateEntity();
        BuildPlayer(scene, m_OtherPeers[i].ID); 
        scene.SetEntityActive(m_OtherPeers[i].ID, false);  
    }
}

#if false
void ClientEventSystem::Update(ECS::Scene& scene, float delta)
{
    auto& component = scene.GetComponent<PhysicsBodyComponent>(0);
    
    input_packet.DataLength = output.str().size() + 1; 
    input_packet.Data = output.str(); 

    EventHandler::Get().Update();

    if (m_NetClient.GetConnected()) 
    { 
        m_NetClient.UpdateNetwork();
        m_NetClient.SendPacket(input_packet, 0, false);  
    }
}
#endif 

void ClientEventSystem::Update(ECS::Scene& scene, float delta) 
{
    EventHandler::Get().Update(); 
    m_CurrentScene = &scene;

    std::uint16_t inputs = GetKeyboardBits(); 
    // Predict Client Movement
    PredictClientState(); 
    InterpolateEntities(); 

    if (m_NetClient.GetConnected())
    {
        // TODO: work on this, unimplemented
        m_NetClient.UpdateNetwork();
        PacketData packet; 
        packet.Type = PT_GAME_UPDATE;
      
        ClientUpdatePayload payload; 
        payload.InputBits = inputs;
        payload.RequestID = m_InputSequenceNumber++; 
        packet.Data = PayloadToString<ClientUpdatePayload>(payload);

        packet.DataLength = packet.Data.size() + 1;
        m_NetClient.SendPacket(packet, 0, false);  
    }
}

void ClientEventSystem::UpdateGame(const std::string &packet_data)
{
    ECS::Scene& scene = *m_CurrentScene; 
    auto payload = PayloadFromString<ServerUpdatePayload>(packet_data);
    // update the client positions 
    std::vector<std::size_t> connected_list; 

    for (auto &client : payload.ClientStates)
    {
        connected_list.push_back(client.ID); 
        auto& component = scene.GetComponent<PhysicsBodyComponent>(m_OtherPeers[client.ID].ID); 
        auto& view = m_OtherPeers[client.ID]; 
        view.LastPosition = { static_cast<float>(component.BoundingBox.x), static_cast<float>(component.BoundingBox.y) };  
        view.LerpPosition = client.Position; 
        scene.SetEntityActive(m_OtherPeers[client.ID].ID); 
    }

    for (std::size_t i = 0; i < m_OtherPeers.size(); ++i)
    {
        // is it in the connected list? if so, make the entity active again
        auto it = std::find(connected_list.begin(), connected_list.end(), i); 
        if (it == connected_list.end())
        {
            // make this client dissappear and reset their position 
            scene.SetEntityActive(m_OtherPeers[i].ID, false); 
            auto& component = scene.GetComponent<PhysicsBodyComponent>(m_OtherPeers[i].ID);
            component = PhysicsBodyComponent { .BoundingBox = component.BoundingBox };
        }
    }
}

void ClientEventSystem::OnRecievePacket(const PacketData &packet)
{
    switch (packet.Type) 
    {
    case PT_GAME_UPDATE: 
        // resync / reconcile / interpolate
        m_LerpTimer.Reset(); 
        UpdateGame(packet.Data); 
        break;     
    }
}

void ClientEventSystem::PredictClientState()
{
    
}

void ClientEventSystem::ReconcileWithServer()
{

}

std::uint16_t ClientEventSystem::GetKeyboardBits()
{
    const std::uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr); 
    std::uint16_t bits = 0;
    bits |= (keyboard_state[SDL_SCANCODE_W]) << 15; 
    bits |= (keyboard_state[SDL_SCANCODE_A]) << 14; 
    bits |= (keyboard_state[SDL_SCANCODE_S]) << 13; 
    bits |= (keyboard_state[SDL_SCANCODE_D]) << 12; 
    bits |= (keyboard_state[SDL_SCANCODE_SPACE]) << 11; 
    return bits; 
}

namespace { constexpr float g_smooth = 1.3f; }

void ClientEventSystem::InterpolateEntities()
{
    for (auto& client : m_OtherPeers)
    {
        auto& component = m_CurrentScene->GetComponent<PhysicsBodyComponent>(client.ID);
        const float T = (std::min)(1.0f, m_LerpTimer.GetDelta() / (1.0f / 10.0f)) * g_smooth; 

        Vector2 new_position = {
            std::lerp(client.Position.X, client.LerpPosition.X, T), 
            std::lerp(client.Position.Y, client.LerpPosition.Y, T)
        }; 

        client.Position = std::move(new_position); 
        component.BoundingBox.x = static_cast<int>(new_position.X);
        component.BoundingBox.y = static_cast<int>(new_position.Y);
    }
}
