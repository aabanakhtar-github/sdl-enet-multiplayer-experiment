#ifndef COLLISON_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "Components.h" 

class CollisionSystem
{
	MAKE_SINGLETON(CollisionSystem)
public:
	CollisionSystem() = default;

	bool IsColliding(AABBColliderComponent& a, AABBColliderComponent& b);
};

#endif // COLLISION_SYSTEM_H