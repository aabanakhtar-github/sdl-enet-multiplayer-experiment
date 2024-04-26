#include "Game.h" 
#include "Util.h"
#include "CreateScenes.h"
#include <thread>
#include "Timer.h"

namespace Client {
    
    Game::Game() 
        : m_GameScenes(), m_SceneIndex(0), m_GraphicsSystem(new GraphicsSystem()), 
            m_PhysicsSystem(new PhysicsSystem()), m_PlayerInputSystem(new ClientEventSystem())
    {
        InitializeLibraries();
    }

    Game::~Game() 
    {
        ShutdownLibraries();
    }

    void Game::Run() 
    {
        Init(); 

        while (true) 
        {
            switch (GlobalAppState::Get().GetAppState()) 
            {
            case AppState::AS_FAIL:
                std::cerr << "Errors have occured! Terminating...." << std::endl; 

                for (auto &error : GlobalAppState::Get().GetError()) 
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
        if (GlobalAppState::Get().GetAppState() == AppState::AS_FAIL) 
        {
            return;  
        }

        CreateGameLevel(m_GameScenes[0]); 

        ECS::SystemManager::Get().RegisterSystems({ m_PlayerInputSystem, m_PhysicsSystem, m_GraphicsSystem });
        ECS::SystemManager::Get().InitAllSystems(m_GameScenes[0]); 

        if (GlobalAppState::Get().GetAppState() != AppState::AS_FAIL) 
        {
            GlobalAppState::Get().SetAppState(AppState::AS_LOOP);
        }   

    }

    void Game::Loop()
    {
        static float delta_time = 0.0f; 
        Timer timer; 
        ECS::SystemManager::Get().UpdateSystems(delta_time, m_GameScenes[0]); 
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
