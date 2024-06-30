#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "ECS.h"
#include "MathFuncs.h"

class PhysicsSystem : public ECS::SystemBase {
public:
  PhysicsSystem() = default;

  void init(ECS::Scene &scene) override;
  void update(ECS::Scene &scene, float delta) override;

private:
  void handleCollisions(ECS::Scene &scene, float delta, ECS::EntityID ID,
                        const std::vector<ECS::EntityID> &physics_bodies);

private:
  constexpr static Vector2 gravity_ = {0, -500.0f};
};

#endif // PHYSICS_SYSTEM_H
