#ifndef GAME_ANIMATION_SYSTEM_H
#define GAME_ANIMATION_SYSTEM_H

#include "ECS.h"

class Timer;
struct AnimationStateMachineComponent;
// Animation state machine

class AnimationSystem : public ECS::SystemBase {
public:
  AnimationSystem() = default;
  void init(ECS::Scene &scene) override;
  void update(ECS::Scene &scene, float delta) override;
};

#endif // GAME_ANIMATION_SYSTEM_H
