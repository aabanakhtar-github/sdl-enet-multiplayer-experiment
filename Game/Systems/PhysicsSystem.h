#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "Util.h"
#include "ECS.h"

class PhysicsSystem : ECS::ISystem
{
	MAKE_SINGLETON(PhysicsSystem)
public:
	PhysicsSystem() = default;
	
	virtual void Init(ECS::Scene& scene) {}
	virtual void Update(ECS::Scene& scene, float delta) override;	
private:
	void ResolveCollisions(PhysicsBodyComponent& component, ECS::SceneView<PhysicsBodyComponent>& scene_view, ECS::Scene& scene);
private:
	constexpr static float kGRAVITY = .3f;
};

#endif // PHYSICS_SYSTEM_H
