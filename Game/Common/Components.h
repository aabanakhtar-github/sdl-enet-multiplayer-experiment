#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "ECS.h"
#include "Window.h"
#include "MathFuncs.h"

//forward declare your components here
struct TextureComponent;
struct PositionComponent;
struct PhysicsBodyComponent;

inline void RegisterComponents(ECS::Scene& scene)
{
	scene.RegisterComponent<TextureComponent>();
	scene.RegisterComponent<PositionComponent>();
	scene.RegisterComponent<PhysicsBodyComponent>();
}

struct TextureComponent : public ECS::Component
{
	Rect SourceRectangle = {};	
	std::string TextureName = "";
	Rect Scale = {}; // overrided by physics body bounding box, if present
};

struct PositionComponent : public ECS::Component
{
	Vector2 Position;
	auto operator <=> (const PositionComponent&) const = default;
};

struct PhysicsBodyComponent : public ECS::Component
{
	Rect BoundingBox = {};
	Vector2 Velocity = {};
	Vector2 Acceleration = {};	
	bool SimulatesPhysics = false;
	
	friend bool operator == (const PhysicsBodyComponent& a, const PhysicsBodyComponent& b) {
		return a.BoundingBox == b.BoundingBox && a.Velocity == b.Velocity
			&& a.Acceleration == b.Acceleration && a.SimulatesPhysics == b.SimulatesPhysics;
	}
};

#endif // COMPONENTS_H