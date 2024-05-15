#ifndef CREATE_SCENES_H
#define CREATE_SCENES_H 

#include "ECS.h"
#include "Components.h"

// TODO: replace with json serialization

inline void BuildPlayer(ECS::Scene& scene, ECS::EntityID player)
{
    scene.AddComponent<TextureComponent>(player) = TextureComponent {
        .SourceRectangle = Rect(0, 0, 100, 100), 
        .TextureName = "foo",
        .Scale = Rect(0, 0, 50, 50)
    }; 
    scene.AddComponent<PhysicsBodyComponent>(player) = PhysicsBodyComponent {
        .BoundingBox = { 0, 0, 50, 50 }, 
        .SimulatesPhysics = false 
    };
}

inline void CreateGameLevel(ECS::Scene& scene, bool server = false) 
{
    scene.RegisterComponent<TextureComponent>();
    scene.RegisterComponent<PhysicsBodyComponent>();
    auto floor = scene.CreateEntity(); 
    scene.AddComponent<TextureComponent>(floor) = TextureComponent {
        .SourceRectangle = Rect(0, 0, 50, 50), 
        .TextureName = "foo",
        .Scale = Rect(0, 0, 700, 700)
    }; 
    scene.AddComponent<PhysicsBodyComponent>(floor) = PhysicsBodyComponent {
        .BoundingBox = { 0, 400, 700, 700 }, 
        .SimulatesPhysics = false
    };


}


#endif