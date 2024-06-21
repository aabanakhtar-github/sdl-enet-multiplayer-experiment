#include "CameraFollowerSystem.h"
#include "ClientEventSystem.h"
#include "Components.h"
#include "GraphicsSystem.h"
#include <iostream>

void CameraFollowerSystem::init(ECS::Scene &scene) {}

void CameraFollowerSystem::update(ECS::Scene &scene, float delta) {
  auto graphics_system = ECS::SystemManager::get().getSystem<GraphicsSystem>();
  auto event_system = ECS::SystemManager::get().getSystem<ClientEventSystem>();
  auto &physics_component =
      scene.getComponent<PhysicsBodyComponent>(event_system->getThisClientID());

  graphics_system->camera.setOffset(
      {physics_component.AABB.x, physics_component.AABB.y});
}
