#include "CollisionSystem.h"

bool CollisionSystem::IsColliding(AABBColliderComponent& a, AABBColliderComponent& b)
{
    if (a.BoundingBox.x < b.BoundingBox.x + b.BoundingBox.w // is the left side before the right side of b? 
        && a.BoundingBox.x + a.BoundingBox.w > b.BoundingBox.x 
        && a.BoundingBox.y < b.BoundingBox.y + b.BoundingBox.h 
        && a.BoundingBox.y + a.BoundingBox.h > b.BoundingBox.y// is the right side after the left side of b?
        )
    {
        return true;
    }
    return false;
}
