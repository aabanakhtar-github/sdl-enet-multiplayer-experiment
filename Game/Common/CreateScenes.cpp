#include "CreateScenes.h"

ECS::EntityID makeEntity(ECS::Scene& scene, const Prototype type, const Vector2& position) {
    switch (type) {
    case Prototype::PLAYER: {
        ECS::EntityID ID = scene.createEntity();
        int x = static_cast<int>(position.x);
        int y = static_cast<int>(position.y);

        scene.addComponent<TextureComponent>(ID) = {
            .SourceRectangle = Rect(x, y, 100, 100), 
            .TextureName = "foo",
            .Scale = Rect(x, y, 50, 50)
        };

        scene.addComponent<PhysicsBodyComponent>(ID) = {
            .BoundingBox = { 0, 0, 50, 50 }, 
            .SimulatesPhysics = false
        }; 

        return ID;  
        break; 
    }
        case Prototype::TILE_1x1_GRASS:
            break;
        case Prototype::TILE_1x1_DIRT:
            break;
        case Prototype::TILE_2x1:
            break;
        case Prototype::TILE_3x1:
            break;
        case Prototype::TILE_3x2:
            break;
    }

    return -1; // invalid
}

void createGameLevel(ECS::Scene& scene) {
    scene.registerComponent<TextureComponent>();
    scene.registerComponent<PhysicsBodyComponent>();
    auto floor = scene.createEntity();

    scene.addComponent<TextureComponent>(floor) = TextureComponent {
        .SourceRectangle = Rect(0, 0, 50, 50), 
        .TextureName = "foo",
        .Scale = Rect(0, 0, 700, 700)
    };

    scene.addComponent<PhysicsBodyComponent>(floor) = PhysicsBodyComponent {
        .BoundingBox = { 0, 400, 700, 700 }, 
        .SimulatesPhysics = false
    };
}