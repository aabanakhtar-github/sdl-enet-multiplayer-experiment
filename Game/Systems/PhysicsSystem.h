#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "Util.h"
#include "ECS.h"

class PhysicsSystem
{
	MAKE_SINGLETON(PhysicsSystem)
public:
	PhysicsSystem() = default;
	
	void Update(ECS::Scene& scene, float delta = 0.0f);	
private:
	void ResolveCollisions(PhysicsBodyComponent& component, ECS::SceneView<PhysicsBodyComponent>& scene_view, ECS::Scene& scene);
private:
	constexpr static float GRAVITY = .1f;
};

#endif // PHYSICS_SYSTEM_H
