#ifndef PLAYER_INPUT_SYSTEM_H
#define PLAYER_INPUT_SYSTEM_H

#include "ECS.h"
#include "Util.h"
#include "SDL.h"

class PlayerInputSystem : ECS::ISystem
{
    MAKE_SINGLETON(PlayerInputSystem)
public:
    PlayerInputSystem() = default;

    virtual void Init(ECS::Scene& scene) override; 
    virtual void Update(ECS::Scene& scene, float deltatime) override; 
};

#endif