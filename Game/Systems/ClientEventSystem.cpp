#include "ClientEventSystem.h"
#include "Components.h" 
#include <cstdint>
#include <iostream>

void ClientEventSystem::Init(ECS::Scene& scene) 
{

    m_NetClient = NetClient(std::bind(&ClientEventSystem::OnRecievePacket, this, std::placeholders::_1)); 
    if (!m_NetClient.GetValid()) 
    {
        GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Cannot initialize network!"); 
    }

    m_NetClient.Connect("127.0.0.1", 7777, 3.0); 

    EventHandler::Get().BindEvent(SDL_KEYDOWN, [&](SDL_Event& e)
    { 
        switch (e.key.keysym.sym) 
        {
            case SDLK_SPACE:
                scene.GetComponent<PhysicsBodyComponent>(0).Acceleration.Y = -20;
                break;
        }  
    });   

    EventHandler::Get().BindEvent(SDL_QUIT, [&](SDL_Event& e) -> void 
    {
        GlobalAppState::Get().SetAppState(AppState::AS_QUIT);
    });
}

void ClientEventSystem::Update(ECS::Scene& scene, float delta)
{
    const std::uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr); 
    auto& component = scene.GetComponent<PhysicsBodyComponent>(0);

    if (keyboard_state[SDL_SCANCODE_D])
    {
        scene.GetComponent<PhysicsBodyComponent>(0).Acceleration.X = 5;
    }
    else if (keyboard_state[SDL_SCANCODE_A])
    {
         scene.GetComponent<PhysicsBodyComponent>(0).Acceleration.X = -5;
    }
    else 
    {
        scene.GetComponent<PhysicsBodyComponent>(0).Velocity.X = 0;
    } 

    if (keyboard_state[SDL_SCANCODE_S])
    {
        component.Acceleration.X = -1;
    }
    else 
    {
        component.Velocity.Y = 0;
    }

    if (keyboard_state[SDL_SCANCODE_W])
    {}

    EventHandler::Get().Update();

    if (m_NetClient.GetConnected()) 
    { 
        m_NetClient.UpdateNetwork();
    }
}

void ClientEventSystem::OnRecievePacket(const PacketData& packet) 
{

}