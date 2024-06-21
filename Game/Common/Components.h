#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <unordered_map>
#include "ECS.h"
#include "Util.h"
#include "MathFuncs.h"
#include "Timer.h"
#include "Window.h"

// TODO: forward declare components here
struct TextureComponent;
struct PositionComponent;
struct PhysicsBodyComponent;
struct AnimationStateMachineComponent;
struct PlayerComponent;
struct TextureGroupComponent;

// getting maps to work with SDL_FRect
namespace std { 

template<>
struct hash<FRect> { 
  size_t operator()(const FRect& rect) const noexcept { 
    size_t seed = hash<float>()(rect.x);
    hash_combine(seed, rect.y); 
    hash_combine(seed, rect.w); 
    hash_combine(seed, rect.h);
    return seed;  
  }
};

}

inline void registerComponents(ECS::Scene &scene) {
  scene.registerComponent<TextureGroupComponent>();
  scene.registerComponent<TextureComponent>();
  scene.registerComponent<PositionComponent>();
  scene.registerComponent<PhysicsBodyComponent>();
  scene.registerComponent<AnimationStateMachineComponent>();
  scene.registerComponent<PlayerComponent>();
}

struct TextureComponent : public ECS::ComponentBase {
  Rect source_rectangle = {};
  std::string texture_name;
  Rect scale = {}; // overrided by physics body bounding box, if present
  SDL_RendererFlip flip = SDL_FLIP_NONE;
};

// multiple textures as a group
struct TextureGroupComponent : public ECS::ComponentBase {
  // std::pair < offset, texture >
  std::unordered_map<FRect, TextureComponent> textures;
};

struct PositionComponent : public ECS::ComponentBase {
  Vector2 position;
  bool cartesian = true;
};

struct PhysicsBodyComponent : public ECS::ComponentBase {
  FRect AABB = {};
  Vector2 velocity;
  bool simulates_physics = false;
  bool grounded = false;

  friend bool operator==(const PhysicsBodyComponent &a,
                         const PhysicsBodyComponent &b) {
    return a.AABB == b.AABB && a.velocity == b.velocity &&
           a.simulates_physics == b.simulates_physics && a.grounded &&
           b.grounded;
  }
};

struct AnimationStateMachineComponent : public ECS::ComponentBase {
  struct Animation {
    std::size_t current_frame;
    std::vector<TextureComponent> frames;
    float seconds_per_frame = 0.5f;
  };

  Timer time_elapsed;
  std::string state = "idle";
  std::string last_state;
  // e.g
  // WALKING -> walk_1, walk_2
  std::unordered_map<std::string, Animation> state_to_animation_frames;
};

// shell component to differentiate players from non-players
struct PlayerComponent : ECS::ComponentBase {
  bool facing_left = false;
};

#endif // COMPONENTS_H
