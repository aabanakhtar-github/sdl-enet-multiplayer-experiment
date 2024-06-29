#include "CreateScenes.h"
#include "Components.h"
#include "ECS.h"

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
                                 .source_rectangle = {128, 70, 20, 35},
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

ECS::EntityID make2x1(ECS::Scene &scene, const Vector2 &position) {
  ECS::EntityID ID = scene.createEntity();
  scene.addComponent<TextureGroupComponent>(ID) = {
      .textures = {{FRect{0, 0, 100, 100},
                    TextureComponent{.source_rectangle{16, 16, 16, 16},
                                     .texture_name = "tileset"}},
                   {FRect{100, 0, 100, 100},
                    TextureComponent{.source_rectangle{16, 16, 16, 16},
                                     .texture_name = "tileset"}}}};
  scene.addComponent<PhysicsBodyComponent>(ID) = {
      .AABB = {position.x, position.y, 200, 100}, .simulates_physics = false};
  return ID;
}

ECS::EntityID make3x1(ECS::Scene &scene, const Vector2 &position) {
  ECS::EntityID ID = scene.createEntity();

  // TODO: finish
  return ID;
}

ECS::EntityID make3x2(ECS::Scene &scene, const Vector2 &position) {
  ECS::EntityID ID = scene.createEntity();

  // TODO: finish
  return ID;
}

} // namespace

ECS::EntityID makeEntity(ECS::Scene &scene, const Proto type,
                         const Vector2 &position) {
  switch (type) {
  case Proto::PLAYER: {
    return ::makePlayer(scene, position);
  }
  case Proto::TILE_2x1:
    return ::make2x1(scene, position);
  // TODO: fill out?
  case Proto::TILE_3x1:
    return ::make3x1(scene, position);
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

  // tiles
  makeEntity(scene, Proto::TILE_2x1, {0, 0});
  makeEntity(scene, Proto::TILE_2x1, {200, 100});
  makeEntity(scene, Proto::TILE_2x1, {-200, 0});
  makeEntity(scene, Proto::TILE_2x1, {-400, 100});
  makeEntity(scene, Proto::TILE_2x1, {-600, 100});
}
