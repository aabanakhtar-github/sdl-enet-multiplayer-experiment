#include "Game.h"
#include "Util.h" 
#include "CreateScenes.h"
#include "Timer.h"
#include "ServerEventSystem.h"
#include "GraphicsSystem.h"
#include "AnimationSystem.h"
#include "PhysicsSystem.h"
#include <iostream>

using namespace std::chrono_literals;

namespace Server {
    
    Game::Game() 
        : arena_(),
          graphics_system_(new GraphicsSystem()),
          physics_system_(new PhysicsSystem()),
          server_event_system_(new ServerEventSystem()),
          animation_system_(new AnimationSystem()) {
        initLibraries();
        registerComponents(arena_);
    }

    Game::~Game() {
        shutdownLibraries();
    }

    void Game::Run() {
        Init(); 

        while (true) {
            switch (GlobalAppState::get().getAppState()) {
            case AppState::AS_FAIL:
                std::cerr << "Errors have occurred! Terminating .... " << std::endl;

                for (auto& error : GlobalAppState::get().getError()) {
                    std::cerr << "Error List: " << error << std::endl; 
                }                    

                goto quit;

            case AppState::AS_LOOP:
                Loop(); 
                break; 

            case AppState::AS_QUIT:
                goto quit; // *GOTO*
            }
        }
quit: 
        Quit();
    }

    void Game::Init() {
        if (GlobalAppState::get().getAppState() == AppState::AS_FAIL) {
            return; 
        }

        server_event_system_->setupServer(7777);

        ECS::SystemManager::get().registerSystem<AnimationSystem>(animation_system_);
        ECS::SystemManager::get().registerSystem<PhysicsSystem>(physics_system_);
        ECS::SystemManager::get().registerSystem<GraphicsSystem>(graphics_system_);
        ECS::SystemManager::get().registerSystem<ServerEventSystem>(server_event_system_);
        ECS::SystemManager::get().initAllSystems(arena_);

        createGameLevel(arena_);

        if (GlobalAppState::get().getAppState() != AppState::AS_FAIL) {
            GlobalAppState::get().setAppState(AppState::AS_LOOP, "");
        } 
    }

    void Game::Quit() {
    }

    void Game::Loop() {
        static float delta_time = 0.0f; 
        
        Timer timer;
        ECS::SystemManager::get().updateSystems(delta_time, arena_);
        delta_time = timer.getDelta();

        if (delta_time < 1.0f / target_fps_) {
            Timer::block(1.0f / target_fps_ - delta_time);
        }

        delta_time = timer.getDelta();
    }

}