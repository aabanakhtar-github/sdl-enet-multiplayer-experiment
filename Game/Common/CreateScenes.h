#ifndef CREATE_SCENES_H
#define CREATE_SCENES_H 

#include "ECS.h"
#include "Components.h"

// TODO: replace with json serialization
inline void CreateGameLevel(ECS::Scene& scene, bool server = false) 
{
    scene.RegisterComponent<TextureComponent>();
    scene.RegisterComponent<PhysicsBodyComponent>();
    // 0 
    auto player = scene.CreateEntity(); 
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


#endif