#include "Components.h" 
#include "Util.h" 
#include "PhysicsSystem.h"

void PhysicsSystem::Update(ECS::Scene& scene, float delta)
{
	ECS::SceneView<PhysicsBodyComponent> physics_bodies(scene);

	for (auto ID : physics_bodies.GetEntities())
	{
		auto& component = scene.GetComponent<PhysicsBodyComponent>(ID);
		if (component.SimulatesPhysics)
		{
			component.Acceleration.Y += kGRAVITY * delta;
		}	

		component.Velocity +=  { component.Acceleration.X * delta, component.Acceleration.Y };
		component.BoundingBox.x += component.Velocity.X; 

		for (auto other : physics_bodies.GetEntities())
		{
			if (ID == other) continue;
			auto& other_component = scene.GetComponent<PhysicsBodyComponent>(other); 
			if (IsColliding(other_component.BoundingBox, component.BoundingBox))
			{
				if (component.Velocity.X > 0)
				{
					component.BoundingBox.x = other_component.BoundingBox.x - component.BoundingBox.w;
				}
				else if (component.Velocity.X < 0)
				{
					component.BoundingBox.x = other_component.BoundingBox.x + other_component.BoundingBox.w;
				}

				component.Velocity.X = 0;
			}
		}	
		
		component.BoundingBox.y += component.Velocity.Y; 
		for (auto other : physics_bodies.GetEntities())
		{
			if (ID == other) continue;
			auto& other_component = scene.GetComponent<PhysicsBodyComponent>(other); 
			if (IsColliding(other_component.BoundingBox, component.BoundingBox))
			{
				if (component.Velocity.Y > 0)
				{
					component.BoundingBox.y = other_component.BoundingBox.y - component.BoundingBox.h;
				}
				else if (component.Velocity.Y < 0)
				{
					component.BoundingBox.y = other_component.BoundingBox.y + other_component.BoundingBox.h;
				}

				component.Velocity.Y = 0;
			}
		}		

		component.Acceleration = { 0, 0 };
	}
}

void PhysicsSystem::ResolveCollisions(PhysicsBodyComponent& component, ECS::SceneView<PhysicsBodyComponent>& scene_view, ECS::Scene& scene)
{
	for (auto ID : scene_view.GetEntities())
	{
		PhysicsBodyComponent& other = scene.GetComponent<PhysicsBodyComponent>(ID);	

		if (component != other)
		{
			// move horizontally first 
			component.BoundingBox.x += component.Velocity.X;	
			if (IsColliding(other.BoundingBox, component.BoundingBox))
			{
				if (component.Velocity.X > 0) 
				{
					// hit left side of something 
					component.BoundingBox.x = other.BoundingBox.x - component.BoundingBox.w;
				}	
				else if (component.Velocity.X < 0)
				{
					// hit right side
					component.BoundingBox.x = other.BoundingBox.x + other.BoundingBox.w;
				}

				component.Velocity.X = 0;
				component.Acceleration.X = 0;
			}

			component.BoundingBox.y += component.Velocity.Y;
			if (IsColliding(other.BoundingBox, component.BoundingBox))
			{
				if (component.Velocity.Y > 0)
				{
					// hit the top side
					component.BoundingBox.y = other.BoundingBox.y - component.BoundingBox.h;
				} 
				else if (component.Velocity.Y < 0)
				{
					// hit the bottom side 
					component.BoundingBox.y = other.BoundingBox.y + other.BoundingBox.h;
				}

				component.Velocity.Y = 0;
				component.Acceleration.Y = 0;
			}

		}
	}
}
