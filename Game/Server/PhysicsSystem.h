#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "Util.h"
#include "ECS.h"
#include "Components.h"

class PhysicsSystem : public ECS::ISystem {
public:
	PhysicsSystem() = default;
	
    void init(ECS::Scene &scene) override;
	void update(ECS::Scene &scene, float delta) override;

private:
	constexpr static float gravity_ = 200.0f;
};

#endif // PHYSICS_SYSTEM_H
