#ifndef CREATE_SCENES_H
#define CREATE_SCENES_H

#include "ECS.h"
#include "MathFuncs.h"

enum class Proto { PLAYER, TILE_2x1, TILE_3x1 };

ECS::EntityID makeEntity(ECS::Scene &scene, Proto type,
                         const Vector2 &position);
void createGameLevel(ECS::Scene &scene);

#endif // CREATE_SCENES_H
