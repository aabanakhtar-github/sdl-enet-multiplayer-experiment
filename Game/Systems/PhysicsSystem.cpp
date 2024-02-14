#include "PhysicsSystem.h"
#include "Components.h" 
#include "Util.h" 

void PhysicsSystem::Update(ECS::Scene& scene, float delta)
{
	ECS::SceneView<PhysicsBodyComponent> physics_bodies(scene);

	for (int ID : physics_bodies.GetEntities())
	{
		auto& component = scene.GetComponent<PhysicsBodyComponent>(ID);
		if (component.SimulatesPhysics)
		{
			if (component.GravityEnabled)
			{
				component.Acceleration.Y += GRAVITY;
			}

			if (component.Position.Y > 500)
			{
				component.Velocity = { 0, 0 };
				component.Acceleration = { 0 , 0 };
			}

			component.Velocity += component.Acceleration;
			component.Position += component.Velocity;

		}


		for (int other : physics_bodies.GetEntities())
		{
			if (IsColliding(component.BoundingBox, scene.GetComponent<PhysicsBodyComponent>(other).BoundingBox))
			{

			}
		}
	}
}