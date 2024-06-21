#include "PhysicsSystem.h"
#include "Components.h"
#include "GraphicsSystem.h"
#include "MathFuncs.h"
#include "ServerEventSystem.h"
#include "Util.h"
#include <iostream>

void PhysicsSystem::update(ECS::Scene &scene, float delta) {
  ECS::SceneView<PhysicsBodyComponent> physics_bodies(scene);

  for (auto &ID : physics_bodies.getEntities()) {
    auto &component = scene.getComponent<PhysicsBodyComponent>(ID);
    if (component.simulates_physics)
      component.velocity += gravity_ * delta;
    handleCollisions(scene, delta, ID, physics_bodies.getEntities());
  }

  auto event_system = ECS::SystemManager::get().getSystem<ServerEventSystem>();
  const auto &players = event_system->getClientToECS_IDArray();

  for (auto &i : event_system->anim_states) {
    i.first = "idle";
  }

  for (std::size_t i = 0; i < players.size(); ++i) {
    if (!scene.entityActive(players[i])) {
      continue;
    }

    auto &component = scene.getComponent<PhysicsBodyComponent>(players[i]);
    if (!component.grounded) {
      event_system->anim_states[i].first = "jump";
    } else if (component.velocity.x != 0) {
      event_system->anim_states[i].first = "run";
    } else {
      event_system->anim_states[i].first = "idle";
    }

    if (component.velocity.x < 0) {
      event_system->anim_states[i].second = true;
    } else if (component.velocity.x > 0) {
      event_system->anim_states[i].second = false;
    }
  }
}

void PhysicsSystem::init(ECS::Scene &scene) {}


void PhysicsSystem::handleCollisions(
    ECS::Scene &scene, float delta, ECS::EntityID ID,
    const std::vector<ECS::EntityID> &physics_bodies) {
  auto graphics_system = ECS::SystemManager::get().getSystem<GraphicsSystem>();
  auto &component = scene.getComponent<PhysicsBodyComponent>(ID);
  component.grounded = false;

  // horizontal collisions
  component.AABB.y += component.velocity.y * delta;

  for (ECS::EntityID other : physics_bodies) {
    if (ID == other) {
      continue; // don't check against self!
    }

    auto &other_component = scene.getComponent<PhysicsBodyComponent>(other);
    Vector2 position =
        graphics_system->camera.transform({component.AABB.x, component.AABB.y});
    FRect rect = {position.x, position.y, component.AABB.w, component.AABB.h};
    Vector2 other_position = graphics_system->camera.transform(
        {other_component.AABB.x, other_component.AABB.y});
    FRect other_rect = {other_position.x, other_position.y,
                        other_component.AABB.w, other_component.AABB.h};

    if (isColliding(rect, other_rect)) {
      if (component.velocity.y < 0) {
        // collision towards floor
        float new_y = other_rect.y - rect.h;
        new_y = graphics_system->camera.inverseTransform({0.0f, new_y}).y;
        component.AABB.y = new_y;
        component.velocity.y = 0;
        component.grounded = true;
      } else if (component.velocity.y > 0) {
        // collision towards ceiling
        float new_y = other_rect.y + rect.h;
        new_y = graphics_system->camera.inverseTransform({0.0f, new_y}).y;
        component.AABB.y = new_y;
        component.velocity.y = 0;
      }
    }
  }

  component.AABB.x += component.velocity.x * delta;

  for (ECS::EntityID other : physics_bodies) {
    if (ID == other) {
      continue; // don't check against self!
    }

    auto &other_component = scene.getComponent<PhysicsBodyComponent>(other);
    Vector2 position =
        graphics_system->camera.transform({component.AABB.x, component.AABB.y});
    FRect rect = {position.x, position.y, component.AABB.w, component.AABB.h};
    Vector2 other_position = graphics_system->camera.transform(
        {other_component.AABB.x, other_component.AABB.y});
    FRect other_rect = {other_position.x, other_position.y,
                        other_component.AABB.w, other_component.AABB.h};

    if (isColliding(rect, other_rect)) {
      if (component.velocity.x < 0) {
        // collision towards left
        float new_x = other_rect.x + other_rect.w;
        new_x = graphics_system->camera.inverseTransform({new_x, 0.0f}).x;
        component.AABB.x = new_x;
        component.velocity.x = 0;
      } else if (component.velocity.x > 0) {
        // collision towards right
        float new_x = other_rect.x - rect.w;
        new_x = graphics_system->camera.inverseTransform({new_x, 0.0f}).x;
        component.AABB.x = new_x;
        component.velocity.x = 0;
      }
    }
  }
}
