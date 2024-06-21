#include "AnimationSystem.h"
#include "Components.h"
#include "ECS.h"
#include <format>
#include <iostream>

void AnimationSystem::init(ECS::Scene &scene) {}

void AnimationSystem::update(ECS::Scene &scene, float delta) {
  ECS::SceneView<TextureComponent, AnimationStateMachineComponent>
      animated_entities(scene);

  for (auto ID : animated_entities.getEntities()) {
    auto &component = scene.getComponent<AnimationStateMachineComponent>(ID);
    auto &anim = component.state_to_animation_frames[component.state];

    assert(component.state_to_animation_frames.contains(component.state) &&
           "No animation available");
    assert(!anim.frames.empty() && "No animations available for state");

    if (component.state != component.last_state) {
      component.time_elapsed.reset();
      anim.current_frame = 0;
    }

    // update animation if timer exceeds seconds per frame
    if (component.time_elapsed.getDelta() > anim.seconds_per_frame) {
      anim.current_frame = (anim.current_frame + 1) % anim.frames.size();
      component.time_elapsed.reset();
    }

    // started a new animation, reset timer
    component.last_state = component.state;
    // set the texture finally
    auto &texture_component = scene.getComponent<TextureComponent>(ID);
    texture_component = anim.frames[anim.current_frame];
  }
}
