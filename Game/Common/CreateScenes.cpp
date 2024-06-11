#include "CreateScenes.h"


namespace {

    ECS::EntityID makePlayer(ECS::Scene& scene, const Vector2& position) {
        ECS::EntityID ID = scene.createEntity();

        scene.addComponent<TextureComponent>(ID) = {
        };
        scene.addComponent<PhysicsBodyComponent>(ID) = {
                .BoundingBox = { 0, 0, 33, 70 },
                .SimulatesPhysics = false
        };
        scene.addComponent<PlayerComponent>(ID);
        scene.addComponent<AnimationStateMachineComponent>(ID) = {
            .state = "idle",
            .last_state = "idle",
            .state_to_animation_frames = {
                { "idle", AnimationStateMachineComponent::Animation {
                        .frames = {
                            TextureComponent {
                                .SourceRectangle = { 33, 70, 20, 35 },
                                .TextureName = "player_idle",
                                .Scale = { 0, 0, 20, 35 }
                            },
                            TextureComponent {
                                .SourceRectangle { 128, 70, 20, 35 },
                                .TextureName = "player_idle",
                                .Scale = {0, 0, 20, 35 }
                            },
                            TextureComponent {
                                .SourceRectangle { 220, 70, 20, 35 },
                                .TextureName = "player_idle",
                                .Scale = { 0, 0, 20, 35 }
                            },
                            TextureComponent {
                                .SourceRectangle { 314, 70, 20, 35 },
                                .TextureName = "player_idle",
                                .Scale = { 0, 0, 20, 35 }
                            },
                            TextureComponent {
                                .SourceRectangle { 407, 70, 20, 35},
                                .TextureName = "player_idle",
                                .Scale = { 0, 0, 20, 35}
                            }
                        },
                        .seconds_per_frame = 0.3f
                    },
                },
                { "jump", AnimationStateMachineComponent::Animation {
                        .frames = {
                            TextureComponent {
                                .SourceRectangle = { 407, 63, 32, 42},
                                .TextureName = "player_attack",
                                .Scale = { 0, 0, 32, 42 }
                            }
                        },
                        .seconds_per_frame = 0.3f
                    }
                },
            }
        };

        return ID;
    }

}

ECS::EntityID makeEntity(ECS::Scene& scene, const Proto type, const Vector2& position) {
    switch (type) {
    case Proto::PLAYER: {
        return makePlayer(scene, position);
    }
    case Proto::TILE_1x1_GRASS:
        break;
    case Proto::TILE_1x1_DIRT:
        break;
    case Proto::TILE_2x1:
        break;
    case Proto::TILE_3x1:
        break;
    case Proto::TILE_3x2:
        break;
    }

    return -1; // invalid
}

void createGameLevel(ECS::Scene& scene) {

    ECS::EntityID background = scene.createEntity();
    scene.addComponent<TextureComponent>(background) = {
       .SourceRectangle = Rect(0, 0, 900, 600),
       .TextureName = "bg",
       .Scale = Rect(0, 0, 900, 600)
    };
    scene.addComponent<PositionComponent>(background) = {
            .Position = Vector2(0, 0)
    };

    ECS::EntityID floor = scene.createEntity();
    scene.addComponent<TextureComponent>(floor) = TextureComponent {
        .SourceRectangle = Rect(0, 0, 50, 50), 
        .TextureName = "foo",
        .Scale = Rect(0, 0, 700, 700)
    };
    scene.addComponent<PhysicsBodyComponent>(floor) = PhysicsBodyComponent {
        .BoundingBox = { 0, 400, 700, 700 }, 
        .SimulatesPhysics = false
    };

    ECS::EntityID floor2 = scene.createEntity();
    scene.addComponent<TextureComponent>(floor2) = TextureComponent {
            .SourceRectangle = Rect(0, 0, 50, 50),
            .TextureName = "foo",
            .Scale = Rect(0, 0, 700, 700)
    };
    scene.addComponent<PhysicsBodyComponent>(floor2) = PhysicsBodyComponent {
            .BoundingBox = { 400, 350, 700, 700 },
            .SimulatesPhysics = false
    };

}