#include "Game.h" 
#include "Util.h"
#include "CreateScenes.h"
#include <thread>
#include <iostream>
#include "Timer.h"

namespace Client {
    
    Game::Game() 
        : m_GameScenes(), m_SceneIndex(0), m_GraphicsSystem(new GraphicsSystem()), 
            m_PhysicsSystem(new PhysicsSystem()), m_PlayerInputSystem(new ClientEventSystem())
    {
        initLibraries();
    }

    Game::~Game() 
    {
        shutdownLibraries();
    }

    void Game::Run() 
    {
        Init(); 

        while (true) 
        {
            switch (GlobalAppState::get().getAppState()) 
            {
            case AppState::AS_FAIL:
                std::cerr << "Errors have occured! Terminating...." << std::endl; 

                for (auto &error : GlobalAppState::get().getError()) 
                {
                    std::cerr << "Error List: " << error << std::endl; 
                }

                goto quit; 
                break;

            case AppState::AS_LOOP: 
                Loop(); 
                break; 

            case AppState::AS_QUIT: 
                goto quit; 
                break; 
            }
        } 

quit:
        Quit(); 
    }

    void Game::Init() 
    {
        // constructor fail => library init failed
        if (GlobalAppState::get().getAppState() == AppState::AS_FAIL) 
        {
            return;  
        }

        CreateGameLevel(m_GameScenes[0]); 

        ECS::SystemManager::get().RegisterSystems({ m_PlayerInputSystem, m_GraphicsSystem });
        ECS::SystemManager::get().InitAllSystems(m_GameScenes[0]); 

        if (GlobalAppState::get().getAppState() != AppState::AS_FAIL) 
        {
            GlobalAppState::get().setAppState(AppState::AS_LOOP);
        }   
        
   }

    void Game::Loop()
    {
        static float delta_time = 0.0f; 
        Timer timer; 
        ECS::SystemManager::get().UpdateSystems(delta_time, m_GameScenes[0]); 
        delta_time = timer.GetDelta(); 

        if (delta_time < 1.0f / m_TargetFPS)
        {
            timer.Block(1.0f / m_TargetFPS - delta_time);
        }

        delta_time = timer.GetDelta();
    }

    void Game::Quit() 
    {
    }

}
