#include "Game.h"
#include "Util.h" 
#include "CreateScenes.h"
#include <chrono> 
#include <thread> 

using namespace std::chrono_literals; 

namespace Server
{
    
    Game::Game() 
        : m_Arena(), m_GraphicsSystem(new GraphicsSystem()), 
            m_PhysicsSystem(new PhysicsSystem()), m_ServerEventSystem(new ServerEventSystem())
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
                std::cerr << "Errors have occured! Terminating .... " << std::endl; 

                for (auto& error : GlobalAppState::Get().GetError())
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
        if (GlobalAppState::Get().GetAppState() == AppState::AS_FAIL) 
        {
            return; 
        }

        m_ServerEventSystem->SetupServer(7777); 

        CreateGameLevel(m_Arena, true);

        ECS::SystemManager::Get().RegisterSystems({ m_ServerEventSystem, m_PhysicsSystem, m_GraphicsSystem });
        ECS::SystemManager::Get().InitAllSystems(m_Arena);  

        if (GlobalAppState::Get().GetAppState() != AppState::AS_FAIL)
        {
            GlobalAppState::Get().SetAppState(AppState::AS_LOOP); 
        } 
    }

    void Game::Quit()
    {
    }

    void Game::Loop() 
    {
        ECS::SystemManager::Get().UpdateSystems(0.16666667, m_Arena); 
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000/60))); 
    }

}