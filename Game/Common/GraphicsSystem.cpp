#include "GraphicsSystem.h"
#include "Components.h"
#include "Texture.h"
#include "Window.h"
#include <iostream>

GraphicsSystem::GraphicsSystem() : camera(), game_window_() {
  if (!game_window_.init("Multiplayer Demo", 900, 600)) {
    GlobalAppState::get().setAppState(AppState::AS_FAIL,
                                      "Couldn't open window!");
  }
}

void GraphicsSystem::init(ECS::Scene &scene) {
  auto protected_load = [this](const std::string &file,
                               const std::string &name) -> void {
    bool success = TextureManager::get().addTexture(game_window_, file, name);

    if (!success) {
      std::cerr << "Failed to load texture " << file
                << " with SDL_ERROR: " << SDL_GetError() << std::endl;
    }
  };

  protected_load("Assets/tileset.png", "tileset");
  protected_load("Assets/background.png", "bg");
  protected_load("Assets/player_idle.png", "player_idle");
  protected_load("Assets/player_run.png", "player_run");
  protected_load("Assets/player_attack.png", "player_attack");
}

void GraphicsSystem::update(ECS::Scene &scene, float delta) {
  game_window_.clear();

  ECS::SceneView<TextureComponent, PositionComponent> regular_textures(scene);
  ECS::SceneView<TextureComponent, PhysicsBodyComponent> physics_textures(
      scene);
  ECS::SceneView<TextureGroupComponent, PhysicsBodyComponent>
      physics_texture_groups(scene);
  TextureManager &manager = TextureManager::get();

  drawNormalEntities(scene, manager, regular_textures.getEntities());
  drawPhysicsEntities(scene, manager, physics_textures.getEntities());
  drawGroupPhysicsEntities(scene, manager,
                           physics_texture_groups.getEntities());

  game_window_.showBuffers();
}

void GraphicsSystem::drawGroupPhysicsEntities(
    ECS::Scene &scene, TextureManager &manager,
    const std::vector<ECS::EntityID> &IDs) {
  for (ECS::EntityID ID : IDs) {
    auto &texture_component = scene.getComponent<TextureGroupComponent>(ID);
    auto &physics_component = scene.getComponent<PhysicsBodyComponent>(ID);
    // get the final position of this entity so that we can use the
    // non-cartesian offsets
    Vector2 transformed =
        camera.transform({physics_component.AABB.x, physics_component.AABB.y});

    // Note: texture_component is just data, not actual component
    for (auto &[offset_rect, texture_component] : texture_component.textures) {
      auto &texture = manager.getTexture(texture_component.texture_name);
      Rect src = texture_component.source_rectangle;
      // calculate offset
      Vector2 offseted = transformed + Vector2(offset_rect.x, offset_rect.y);
      FRect destination = {offseted.x, offseted.y, offset_rect.w,
                           offset_rect.h};

      game_window_.renderTexture(texture, &src, &destination);
    }
  }
}

void GraphicsSystem::drawNormalEntities(ECS::Scene &scene,
                                        TextureManager &manager,
                                        const std::vector<ECS::EntityID> &IDs) {
  for (ECS::EntityID ID : IDs) {
    auto &texture_component = scene.getComponent<TextureComponent>(ID);
    auto &position_component = scene.getComponent<PositionComponent>(ID);
    TextureData &texture = manager.getTexture(texture_component.texture_name);

    if (!texture.getValid()) {
      std::cerr << "Cannot render texture named "
                << texture_component.texture_name
                << "! SDL_Error: " << SDL_GetError() << std::endl;
      continue;
    }

    Rect src = texture_component.source_rectangle;
    FRect destination;

    if (position_component.cartesian) {
      Vector2 transformed = camera.transform(
          {position_component.position.x, position_component.position.y});
      destination = {transformed.x, transformed.y, static_cast<float>(src.w),
                     static_cast<float>(src.h)};
    } else {
      destination = {position_component.position.x,
                     position_component.position.y, static_cast<float>(src.w),
                     static_cast<float>(src.h)};
    }

    game_window_.renderTexture(texture, &src, &destination,
                               texture_component.flip);

    if constexpr (draw_debug_rects_) {
      game_window_.drawRect(destination);
    }
  }
}

// physics entities don't have position component, handle separately
void GraphicsSystem::drawPhysicsEntities(
    ECS::Scene &scene, TextureManager &manager,
    const std::vector<ECS::EntityID> &IDs) {
  for (ECS::EntityID ID : IDs) {
    auto &texture_component = scene.getComponent<TextureComponent>(ID);
    auto &physics_component = scene.getComponent<PhysicsBodyComponent>(ID);
    TextureData &texture = manager.getTexture(texture_component.texture_name);

    if (!texture.getValid()) {
      std::cerr << "Cannot render texture named "
                << texture_component.texture_name
                << "! SDL_Error: " << SDL_GetError() << std::endl;
      continue;
    }

    Rect src = texture_component.source_rectangle;
    Vector2 transformed =
        camera.transform({static_cast<float>(physics_component.AABB.x),
                          static_cast<float>(physics_component.AABB.y)});
    FRect destination = {transformed.x, transformed.y,
                         static_cast<float>(physics_component.AABB.w),
                         static_cast<float>(physics_component.AABB.h)};

    if (scene.hasComponent<PlayerComponent>(ID)) {
      auto &player_component = scene.getComponent<PlayerComponent>(ID);
      game_window_.renderTexture(
          texture, &src, &destination,
          player_component.facing_left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    } else {
      game_window_.renderTexture(texture, &src, &destination);
    }

    if constexpr (draw_debug_rects_) {
      game_window_.drawRect(destination);
    }
  }
}
