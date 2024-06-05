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

void ClientEventSystem::init(ECS::Scene &scene) {
    m_NetClient = NetClient(std::bind(&ClientEventSystem::OnRecievePacket, this, std::placeholders::_1)); 
    
    if (!m_NetClient.getValid()) {
        GlobalAppState::get().setAppState(AppState::AS_FAIL, "Cannot initialize network!"); 
    }

    // TODO refactor into reusable function that allows game to persist across multiple servers
    m_NetClient.connectTo("127.0.0.1", 7777, 3.0); 

    EventHandler::get().bindEvent(SDL_KEYDOWN, [&](SDL_Event& e) -> void {
        switch (e.key.keysym.sym) {
        case SDLK_SPACE: {
            PacketData packet; 
            packet.type = PT_CLIENT_JUMP; 
            packet.data_size = 0; 
            m_NetClient.sendPacket(packet, 0, true);
            break;
        }
       default:
            break; 
        }  
    });   

    EventHandler::get().bindEvent(SDL_QUIT, [&](SDL_Event& e) -> void {
        GlobalAppState::get().setAppState(AppState::AS_QUIT);
    });

    // Create Players 
    for (auto & m_OtherPeer : m_OtherPeers) {
        m_OtherPeer.ID = makeEntity(scene, Prototype::PLAYER, Vector2(0, 0));
        scene.setEntityActive(m_OtherPeer.ID, false);
    }
 
}

#if false
void ClientEventSystem::Update(ECS::Scene& scene, float delta)
{
    auto& component = scene.GetComponent<PhysicsBodyComponent>(0);
    
    input_packet.DataLength = output.str().size() + 1; 
    input_packet.Data = output.str(); 

    EventHandler::get().Update();

    if (m_NetClient.getConnected()) 
    { 
        m_NetClient.UpdateNetwork();
        m_NetClient.SendPacket(input_packet, 0, false);  
    }
}
#endif 

void ClientEventSystem::update(ECS::Scene &scene, float delta)
{
    EventHandler::get().update();
    m_CurrentScene = &scene;

    std::uint16_t inputs = GetKeyboardBits(); 
    // Predict Client Movement
    PredictClientState(); 
    InterpolateEntities(); 

    if (m_NetClient.getConnected())
    {
        // TODO: work on this, unimplemented
        m_NetClient.updateNetwork();
        PacketData packet; 
        packet.type = PT_GAME_UPDATE;
      
        ClientUpdatePayload payload; 
        payload.input_bits = inputs;
        packet.data = payloadToString<ClientUpdatePayload>(payload);

        packet.data_size = packet.data.size() + 1;
        m_NetClient.sendPacket(packet, 0, false);  
    }
}

void ClientEventSystem::UpdateGame(const std::string &packet_data)
{
    ECS::Scene& scene = *m_CurrentScene;
    auto payload = payloadFromString<ServerUpdatePayload>(packet_data);
    // update the client positions 
    std::vector<std::size_t> connected_list; 

    for (auto &client : payload.client_states)
    {
        connected_list.push_back(client.ID); 
        auto& component = scene.getComponent<PhysicsBodyComponent>(m_OtherPeers[client.ID].ID);
        auto& view = m_OtherPeers[client.ID]; 
        view.LastPosition = { static_cast<float>(component.BoundingBox.x), static_cast<float>(component.BoundingBox.y) };  
        view.LerpPosition = client.position;
        scene.setEntityActive(m_OtherPeers[client.ID].ID);
    }

    for (std::size_t i = 0; i < m_OtherPeers.size(); ++i)
    {
        // is it in the connected list? if so, make the entity active again
        auto it = std::find(connected_list.begin(), connected_list.end(), i); 
        if (it == connected_list.end())
        {
            // make this client dissappear and reset their position 
            scene.setEntityActive(m_OtherPeers[i].ID, false);
            auto& component = scene.getComponent<PhysicsBodyComponent>(m_OtherPeers[i].ID);
            component = PhysicsBodyComponent { .BoundingBox = component.BoundingBox };
        }
    }
}

void ClientEventSystem::OnRecievePacket(const PacketData &packet)
{
    switch (packet.type) 
    {
    case PT_GAME_UPDATE: 
        // resync / reconcile / interpolate
        m_LerpTimer.reset();
        UpdateGame(packet.data); 
        break;     
    }
}

void ClientEventSystem::PredictClientState()
{
    
}

void ClientEventSystem::ReconcileWithServer()
{

}

std::uint16_t ClientEventSystem::GetKeyboardBits() // NOLINT(*-convert-member-functions-to-static)
{
    const std::uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr); 
    std::uint16_t bits = 0;
    bits |= (keyboard_state[SDL_SCANCODE_A]) << 14; 
    bits |= (keyboard_state[SDL_SCANCODE_D]) << 13; 
    return bits; 
}

void ClientEventSystem::InterpolateEntities()
{
    static constexpr float smooth = 1.3f;

    for (auto& client : m_OtherPeers)
    {
        auto& component = m_CurrentScene->getComponent<PhysicsBodyComponent>(client.ID);
        const float T = (std::min)(1.0f, m_LerpTimer.getDelta() / (1.0f / 10.0f)) * smooth;

        Vector2 new_position = {
            std::lerp(client.position.x, client.LerpPosition.x, T),
            std::lerp(client.position.y, client.LerpPosition.y, T)
        }; 

        client.position = std::move(new_position); 
        component.BoundingBox.x = static_cast<int>(new_position.x);
        component.BoundingBox.y = static_cast<int>(new_position.y);
    }
}
