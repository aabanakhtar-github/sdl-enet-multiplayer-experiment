#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "ECS.h"
#include "Window.h"
#include "VectorMath.h"

//forward declare your components here
class TextureComponent;
class PositionComponent;
class PhysicsBodyComponent;

inline void RegisterComponents(ECS::Scene& scene)
{
	scene.RegisterComponent<TextureComponent>();
	scene.RegisterComponent<PositionComponent>();
	scene.RegisterComponent<PhysicsBodyComponent>();
}

class TextureComponent : public ECS::Component
{
public:
	DECLARE_MEMBER_AND_ACCESSOR(TextureComponent, float, Scale, 1.f)
	DECLARE_MEMBER_AND_ACCESSOR(TextureComponent, Rect, SourceRectangle, Rect())
	DECLARE_MEMBER_AND_ACCESSOR(TextureComponent, std::string, TextureName, "")
};

/* 
*	Don't use with PhysicsBodyComponent on Entity! 
*/
class PositionComponent : public ECS::Component
{
public:
	DECLARE_MEMBER_AND_ACCESSOR(PositionComponent, float, X, 0.f)
	DECLARE_MEMBER_AND_ACCESSOR(PositionComponent, float, Y, 0.f)
};

class PhysicsBodyComponent : public ECS::Component
{
public:
	DECLARE_MEMBER_AND_ACCESSOR(PhysicsBodyComponent, Rect, BoundingBox, Rect(0, 0, 0, 0))
	DECLARE_MEMBER_AND_ACCESSOR(PhysicsBodyComponent, Vector2, Position, Vector2(0, 0))
	DECLARE_MEMBER_AND_ACCESSOR(PhysicsBodyComponent, Vector2, Velocity, Vector2(0, 0))
	DECLARE_MEMBER_AND_ACCESSOR(PhysicsBodyComponent, Vector2, Acceleration, Vector2(0, 0))
	DECLARE_MEMBER_AND_ACCESSOR(PhysicsBodyComponent, bool, GravityEnabled, true)
	DECLARE_MEMBER_AND_ACCESSOR(PhysicsBodyComponent, bool, SimulatesPhysics, true)
};

#endif // COMPONENTS_H
