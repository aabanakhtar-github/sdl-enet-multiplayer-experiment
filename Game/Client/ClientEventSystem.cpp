#include "ClientEventSystem.h"
#include "Components.h"
#include <cstdint>
#include <sstream> 
#include <iostream>

void ClientEventSystem::Init(ECS::Scene& pscene) 
{
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

    if (m_NetClient.GetConnected())
    {
        m_NetClient.UpdateNetwork();
    }
}

void ClientEventSystem::OnRecievePacket(const PacketData& packet) 
{
    switch (packet.Type) 
    {
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
