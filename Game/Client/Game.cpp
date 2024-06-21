#include "Game.h"
#include "AnimationSystem.h"
#include "CameraFollowerSystem.h"
#include "ClientEventSystem.h"
#include "CreateScenes.h"
#include "GraphicsSystem.h"
#include "Timer.h"
#include "Util.h"
#include <iostream>

namespace Client {

Game::Game()
    : game_scenes_(), scene_index_(0), graphics_system_(new GraphicsSystem()),
      player_input_system_(new ClientEventSystem()),
      animation_system_(new AnimationSystem()),
      camera_follower_system_(new CameraFollowerSystem()) {
  initLibraries();
  registerComponents(game_scenes_[0]);
}

Game::~Game() { shutdownLibraries(); }

void Game::Run() {
  init();

  while (true) {
    switch (GlobalAppState::get().getAppState()) {
    case AppState::AS_FAIL:
      std::cerr << "Errors have occurred! Terminating...." << std::endl;

      for (auto &error : GlobalAppState::get().getError()) {
        std::cerr << "Error List: " << error << std::endl;
      }

      goto quit;
      break;

    case AppState::AS_LOOP:
      loop();
      break;

    case AppState::AS_QUIT:
      goto quit;
      break;
    }
  }

quit:
  quit();
}

void Game::init() {
  // constructor fail => library init failed
  if (GlobalAppState::get().getAppState() == AppState::AS_FAIL) {
    return;
  }

  ECS::SystemManager::get().registerSystem<AnimationSystem>(animation_system_);
  ECS::SystemManager::get().registerSystem<GraphicsSystem>(graphics_system_);
  ECS::SystemManager::get().registerSystem<ClientEventSystem>(
      player_input_system_);
  ECS::SystemManager::get().registerSystem<CameraFollowerSystem>(
      camera_follower_system_);
  ECS::SystemManager::get().initAllSystems(game_scenes_[0]);

  createGameLevel(game_scenes_[0]);

  if (GlobalAppState::get().getAppState() != AppState::AS_FAIL) {
    GlobalAppState::get().setAppState(AppState::AS_LOOP);
  }
}

void Game::loop() {
  static float delta_time = 0.0f;
  Timer timer;
  ECS::SystemManager::get().updateSystems(delta_time, game_scenes_[0]);
  delta_time = timer.getDelta();

  if (delta_time < 1.0f / target_fps_) {
    Timer::block(1.0f / target_fps_ - delta_time);
  }

  delta_time = timer.getDelta();
}

void Game::quit() {}

} // namespace Client
