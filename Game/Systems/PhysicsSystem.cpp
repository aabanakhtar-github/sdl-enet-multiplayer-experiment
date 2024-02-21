#include "Components.h" 
#include "Util.h" 
#include "PhysicsSystem.h"

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
		
			component.Velocity += component.Acceleration;
			component.BoundingBox.x += (int)component.Velocity.X;
			component.BoundingBox.y += (int)component.Velocity.Y;

			ResolveCollisions(component, physics_bodies, scene);

			component.Acceleration = { 0, 0 };
		}
	}
}

void PhysicsSystem::ResolveCollisions(PhysicsBodyComponent& component, ECS::SceneView<PhysicsBodyComponent>& scene_view, ECS::Scene& scene)
{
	for (auto ID : scene_view.GetEntities())
	{
		PhysicsBodyComponent other = scene.GetComponent<PhysicsBodyComponent>(ID);
		// don't check if self collides with self
		if (component == other) continue;
	
		if (IsColliding(component.BoundingBox, other.BoundingBox))
		{
			if (component.Velocity.X > 0)
			{
				component.BoundingBox.x = other.BoundingBox.x - component.BoundingBox.w;
			}
			else if (component.Velocity.X < 0)
			{
				component.BoundingBox.x = other.BoundingBox.x + other.BoundingBox.w;	
			}
		
			// seperate axis, in order to simplify resolution
			if (component.Velocity.Y > 0)
			{
				component.BoundingBox.y = other.BoundingBox.y - component.BoundingBox.h;
			}
			else if (component.Velocity.Y < 0)
			{
				component.BoundingBox.y = other.BoundingBox.y + other.BoundingBox.h;
			}

			component.Velocity = { 0, 0 };
			component.Acceleration = { 0, 0 };
		}
	}
}
