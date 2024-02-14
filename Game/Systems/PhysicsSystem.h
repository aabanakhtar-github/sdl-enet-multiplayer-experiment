#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "Util.h"

class PhysicsSystem
{
	MAKE_SINGLETON(PhysicsSystem)
public:
	PhysicsSystem() = default;
	
	void Update(ECS::Scene& scene, float delta = 0.0f);	

private:
	constexpr static float GRAVITY = .24f;
};

#endif // PHYSICS_SYSTEM_H
