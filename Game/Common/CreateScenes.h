#ifndef CREATE_SCENES_H
#define CREATE_SCENES_H 

#include "ECS.h"
#include "Components.h"

enum class Proto {
    PLAYER, 
    TILE_1x1_GRASS, 
    TILE_1x1_DIRT, 
    TILE_2x1, 
    TILE_3x1, 
    TILE_3x2
};

ECS::EntityID makeEntity(ECS::Scene& scene, Proto type, const Vector2& position);
void createGameLevel(ECS::Scene& scene);

#endif // CREATE_SCENES_H