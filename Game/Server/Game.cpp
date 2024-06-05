#include "Game.h"
#include "Util.h" 
#include "CreateScenes.h"
#include "Timer.h"
#include <chrono> 

using namespace std::chrono_literals; 

namespace Server
{
    
    Game::Game() 
        : m_Arena(), m_GraphicsSystem(new GraphicsSystem()), 
            m_PhysicsSystem(new PhysicsSystem()), m_ServerEventSystem(new ServerEventSystem(m_Arena))
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
                std::cerr << "Errors have occured! Terminating .... " << std::endl; 

                for (auto& error : GlobalAppState::get().getError())
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
        if (GlobalAppState::get().getAppState() == AppState::AS_FAIL) 
        {
            return; 
        }

        m_ServerEventSystem->SetupServer(7777);

        createGameLevel(m_Arena);

        ECS::SystemManager::get().RegisterSystems({ m_ServerEventSystem, m_PhysicsSystem, m_GraphicsSystem });
        ECS::SystemManager::get().InitAllSystems(m_Arena);  

        if (GlobalAppState::get().getAppState() != AppState::AS_FAIL)
        {
            GlobalAppState::get().setAppState(AppState::AS_LOOP, ""); 
        } 
    }

    void Game::Quit()
    {
    }

    void Game::Loop() 
    {
        static float delta_time = 0.0f; 
        
        Timer timer;
        ECS::SystemManager::get().UpdateSystems(delta_time, m_Arena); 

        delta_time = timer.getDelta();
        if (delta_time < 1.0 / m_TargetFPS)
            timer.Block(1.0 / m_TargetFPS - delta_time); 

        delta_time = timer.getDelta();
    }

}