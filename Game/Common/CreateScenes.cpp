#include "CreateScenes.h"

ECS::EntityID MakeEntity(ECS::Scene& scene, const Prototype type, const Vector2& position)
{
    switch (type) 
    {
    case Prototype::PLAYER: 
    {
        ECS::EntityID ID = scene.CreateEntity(); 
        int x = static_cast<int>(position.X); 
        int y = static_cast<int>(position.Y); 

        scene.AddComponent<TextureComponent>(ID) = { 
            .SourceRectangle = Rect(x, y, 100, 100), 
            .TextureName = "foo",
            .Scale = Rect(x, y, 50, 50)
        };

        scene.AddComponent<PhysicsBodyComponent>(ID) = { 
            .BoundingBox = { 0, 0, 50, 50 }, 
            .SimulatesPhysics = false
        }; 

        return ID;  
        break; 
    }
    }

    return -1; // invalid
}

void CreateGameLevel(ECS::Scene& scene) 
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