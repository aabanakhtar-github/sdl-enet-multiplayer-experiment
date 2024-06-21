#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include "ECS.h"
#include <array>

class CameraFollowerSystem;
class GraphicsSystem;
class PhysicsSystem;
class ClientEventSystem;
class AnimationSystem;

// game encapsulation class
namespace Client {

class Game {
public:
  explicit Game();
  ~Game();

  void Run();

private:
  void init();
  void loop();
  void quit();

private:
  static constexpr float target_fps_ = 60.0f;
  std::array<ECS::Scene, 1> game_scenes_;
  std::size_t scene_index_;
  GraphicsSystem *graphics_system_;
  ClientEventSystem *player_input_system_;
  AnimationSystem *animation_system_;
  CameraFollowerSystem *camera_follower_system_;
};

} // namespace Client

#endif
