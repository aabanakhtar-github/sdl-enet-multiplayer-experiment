#include "PlayerInputSystem.h"
#include "EventHandler.h"
#include "Components.h" 
#include <cstdint>
#include <iostream>

void PlayerInputSystem::Init(ECS::Scene& scene) 
{
    EventHandler::Get().BindEvent(SDL_KEYDOWN, [&](SDL_Event& e){ 
        switch (e.key.keysym.sym) 
        {
            case SDLK_SPACE:
                scene.GetComponent<PhysicsBodyComponent>(0).Acceleration.Y = -20;
                break;
        }  
    });   
}

void PlayerInputSystem::Update(ECS::Scene& scene, float delta)
{
    const std::uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr);
    auto& component = scene.GetComponent<PhysicsBodyComponent>(2);

    if (keyboard_state[SDL_SCANCODE_D])
    {
         scene.GetComponent<PhysicsBodyComponent>(0).Acceleration.X = 2;
    }
    else 
    {
        scene.GetComponent<PhysicsBodyComponent>(0).Velocity.X = 0;
    }

    if (keyboard_state[SDL_SCANCODE_A])
    {
         scene.GetComponent<PhysicsBodyComponent>(0).Acceleration.X = -2;
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




}