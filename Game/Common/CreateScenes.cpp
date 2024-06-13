#include "CreateScenes.h"


namespace {

    ECS::EntityID makePlayer(ECS::Scene& scene, const Vector2& position) {
        ECS::EntityID ID = scene.createEntity();

        scene.addComponent<TextureComponent>(ID) = {
        };
        scene.addComponent<PhysicsBodyComponent>(ID) = {
                .AABB = { -450, 200, 50, 100 },
                .simulates_physics = true
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
                            },
                            TextureComponent {
                                .SourceRectangle { 128, 70, 20, 35 },
                                .TextureName = "player_idle",
                            },
                            TextureComponent {
                                .SourceRectangle { 220, 70, 20, 35 },
                                .TextureName = "player_idle",
                            },
                            TextureComponent {
                                .SourceRectangle { 314, 70, 20, 35 },
                                .TextureName = "player_idle",
                            },
                            TextureComponent {
                                .SourceRectangle { 407, 70, 20, 35 },
                                .TextureName = "player_idle",
                            }
                        },
                        .seconds_per_frame = 0.3f
                    },
                },
                { "jump", AnimationStateMachineComponent::Animation {
                        .frames = {
                            TextureComponent {
                                .SourceRectangle = { 407, 63, 33, 43 },
                                .TextureName = "player_attack",
                            }
                        },
                        .seconds_per_frame = 0.3f
                    }},
                { "run", AnimationStateMachineComponent::Animation {
                        .frames {
                            TextureComponent {
                                .SourceRectangle = { 33, 74, 31, 31 },
                                .TextureName = "player_run"
                            },
                            TextureComponent {
                                .SourceRectangle = { 125, 74, 31, 31 },
                                .TextureName = "player_run"
                            },
                            TextureComponent {
                                .SourceRectangle { 219, 74, 31, 31 },
                                .TextureName = "player_run"
                            },
                            TextureComponent {
                                .SourceRectangle { 312, 75, 31, 31 },
                                .TextureName = "player_run"
                            }
                        },
                        .seconds_per_frame = 0.1f
                    }
                }
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
        .Position = { -450, 300 }
    };

    ECS::EntityID floor = scene.createEntity();
    scene.addComponent<TextureComponent>(floor) = TextureComponent {
        .SourceRectangle = Rect(0, 0, 50, 50), 
        .TextureName = "foo",
        .Scale = Rect(0, 0, 700, 700)
    };
    scene.addComponent<PhysicsBodyComponent>(floor) = PhysicsBodyComponent {
        .AABB = {-450, 0, 700, 700 },
        .simulates_physics = false
    };

}