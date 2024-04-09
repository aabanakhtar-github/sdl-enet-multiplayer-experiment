#include "ClientEventSystem.h"
#include "Components.h" 
#include <cstdint>
#include <sstream> 
#include <iostream>


static ECS::Scene* scene = nullptr; 

void ClientEventSystem::Init(ECS::Scene& pscene) 
{
    scene = &pscene; 
    m_NetClient = NetClient(std::bind(&ClientEventSystem::OnRecievePacket, this, std::placeholders::_1)); 
    if (!m_NetClient.GetValid()) 
    {
        GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Cannot initialize network!"); 
    }

// TODO refactor into reusable function that allows game to persist across multiple servers
    m_NetClient.Connect("127.0.0.1", 7777, 3.0); 

    EventHandler::Get().BindEvent(SDL_KEYDOWN, [&](SDL_Event& e)
    { 
        switch (e.key.keysym.sym) 
        {
            case SDLK_SPACE:
                scene->GetComponent<PhysicsBodyComponent>(0).Acceleration.Y = -20;
                break;
        }  
    });   

    EventHandler::Get().BindEvent(SDL_QUIT, [&](SDL_Event& e) -> void 
    {
        GlobalAppState::Get().SetAppState(AppState::AS_QUIT);
    });
}

#if false
void ClientEventSystem::Update(ECS::Scene& scene, float delta)
{
    const std::uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr); 
    auto& component = scene.GetComponent<PhysicsBodyComponent>(0);

    auto input_packet = PacketData{
        .ID = 0, 
        .Type = PT_GAME_UPDATE, 
    };

    std::ostringstream output; 

    if (keyboard_state[SDL_SCANCODE_D])
    {
        output << "1 "; 
        component.Velocity.X = 15 * 1 * delta; 
    }
    else if (keyboard_state[SDL_SCANCODE_A])
    {
        output << "-1 "; 
        component.Velocity.X = 15 * -1 * delta; 
    }
    else 
    {
        output << "0 "; 
        component.Velocity.X = 0; 
    } 

    if (keyboard_state[SDL_SCANCODE_W]) 
    {
        output << "-1 "; 
        component.Velocity.Y = 15 * 1 * delta; 
    }
    else if (keyboard_state[SDL_SCANCODE_S])
    {
        output << "1 "; 
        component.Velocity.Y = 15 * -1 * delta; 
    }
    else 
    {
        component.Velocity.Y = 0; 
        output << "0 "; 
    }

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

    // Predict Client Movement
    PredictClientState(); 
    // Interp other members 
    InterpolateEntities(); 
}

void ClientEventSystem::OnRecievePacket(const PacketData& packet) 
{
    switch (packet.Type) 
    {
    case PT_HANDSHAKE:
        // XOR Salts and send back to server
        break; 

    case PT_HANDSHAKE_RESULT:
    {    // do stuff based on whether it's rejected or accepted (client ids)
        auto payload = PayloadFromString<HandshakeAcceptRejectPayload>(packet.Data);
        if (payload.Accepted)      
        {
        } 
        else 
        {   

        }
        break; 
    }
    case PT_GAME_UPDATE: 
        // resync / reconcile / interpolate
        ReconcileWithServer(); 
        break;     
    }
}

void ClientEventSystem::PredictClientState()
{
    
}

void ClientEventSystem::ReconcileWithServer()
{

}

void ClientEventSystem::InterpolateEntities()
{

}
