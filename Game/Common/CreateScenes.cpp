#include "CreateScenes.h"

namespace {
// builder functions
ECS::EntityID makePlayer(ECS::Scene &scene, const Vector2 &position) {
  ECS::EntityID ID = scene.createEntity();

  scene.addComponent<TextureComponent>(ID) = {};
  scene.addComponent<PhysicsBodyComponent>(ID) = {.AABB = {-450, 200, 50, 100},
                                                  .simulates_physics = true};
  scene.addComponent<PlayerComponent>(ID);
  scene.addComponent<AnimationStateMachineComponent>(ID) = {
      .state = "idle",
      .last_state = "idle",
      .state_to_animation_frames = {
          {
              "idle",
              AnimationStateMachineComponent::Animation{
                  .frames = {TextureComponent{
                                 .source_rectangle = {33, 70, 20, 35},
                                 .texture_name = "player_idle",
                             },
                             TextureComponent{
                                 .source_rectangle{128, 70, 20, 35},
                                 .texture_name = "player_idle",
                             },
                             TextureComponent{
                                 .source_rectangle{220, 70, 20, 35},
                                 .texture_name = "player_idle",
                             },
                             TextureComponent{
                                 .source_rectangle{314, 70, 20, 35},
                                 .texture_name = "player_idle",
                             },
                             TextureComponent{
                                 .source_rectangle{407, 70, 20, 35},
                                 .texture_name = "player_idle",
                             }},
                  .seconds_per_frame = 0.3f},
          },
          {"jump",
           AnimationStateMachineComponent::Animation{
               .frames = {TextureComponent{
                   .source_rectangle = {407, 63, 33, 43},
                   .texture_name = "player_attack",
               }},
               .seconds_per_frame = 0.3f}},
          {"run",
           AnimationStateMachineComponent::Animation{
               .frames{TextureComponent{.source_rectangle = {33, 74, 31, 31},
                                        .texture_name = "player_run"},
                       TextureComponent{.source_rectangle = {125, 74, 31, 31},
                                        .texture_name = "player_run"},
                       TextureComponent{.source_rectangle{219, 74, 31, 31},
                                        .texture_name = "player_run"},
                       TextureComponent{.source_rectangle{312, 75, 31, 31},
                                        .texture_name = "player_run"}},
               .seconds_per_frame = 0.1f}}}};

  return ID;
}

ECS::EntityID make3x2(ECS::Scene &scene, const Vector2 &position) {
  ECS::EntityID ID = scene.createEntity();

  return ID;
}

} // namespace

ECS::EntityID makeEntity(ECS::Scene &scene, const Proto type,
                         const Vector2 &position) {
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
    return make3x2(scene, position);
  }

  return -1; // invalid
}

void createGameLevel(ECS::Scene &scene) {
  ECS::EntityID background = scene.createEntity();
  scene.addComponent<TextureComponent>(background) = {
      .source_rectangle = Rect(0, 0, 900, 600),
      .texture_name = "bg",
      .scale = Rect(0, 0, 900, 600)};
  scene.addComponent<PositionComponent>(background) = {.position = {0.0f, 0.0f},
                                                       .cartesian = false};

  ECS::EntityID floor = scene.createEntity();
  scene.addComponent<TextureComponent>(floor) =
      TextureComponent{.source_rectangle = Rect(0, 0, 50, 50),
                       .texture_name = "foo",
                       .scale = Rect(0, 0, 700, 700)};
  scene.addComponent<PhysicsBodyComponent>(floor) = PhysicsBodyComponent{
      .AABB = {-450, 0, 700, 700}, .simulates_physics = false};
}
