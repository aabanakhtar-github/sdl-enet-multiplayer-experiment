#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include "Util.h"
#include "ECS.h"
#include "Components.h"

class PhysicsSystem : public ECS::ISystem
{
public:
	PhysicsSystem() = default;
	
	virtual void init(ECS::Scene &scene) {}
	virtual void update(ECS::Scene &scene, float delta) override;
private:
	void ResolveCollisions(PhysicsBodyComponent& component, ECS::SceneView<PhysicsBodyComponent>& scene_view, ECS::Scene& scene);
private:
	constexpr static float m_Gravity = 50.0f;
};

#endif // PHYSICS_SYSTEM_H
