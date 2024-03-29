#include "Game.h"
#include "Util.h" 
#include "CreateScenes.h"

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

        CreateGameLevel(m_Arena, true)
    }

    void Game::Quit()
    {

    }

    void Game::Loop() 
    {

    }

}