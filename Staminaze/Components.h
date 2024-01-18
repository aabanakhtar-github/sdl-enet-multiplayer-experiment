#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "All.h"

class TextureComponent : public ECS::Component
{
public:
	DECLARE_MEMBER_AND_ACCESSOR(TextureComponent, float, Scale, 1.f)
	DECLARE_MEMBER_AND_ACCESSOR(TextureComponent, Rect, SourceRectangle, Rect())
	DECLARE_MEMBER_AND_ACCESSOR(TextureComponent, std::string, TextureName, "")
};

class PositionComponent : public ECS::Component
{
public:
	DECLARE_MEMBER_AND_ACCESSOR(PositionComponent, float, X, 0.f)
	DECLARE_MEMBER_AND_ACCESSOR(PositionComponent, float, Y, 0.f)
};

class AABBColliderComponent : public ECS::Component
{
public:
	DECLARE_MEMBER_AND_ACCESSOR(AABBColliderComponent, Rect, BoundingBox, Rect())
};

#endif
