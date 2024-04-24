#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "ECS.h"
#include "GraphicsSystem.h"
#include "PhysicsSystem.h"
#include "ServerEventSystem.h"

namespace Server
{

    class Game 
    {
    public: 
        explicit Game();
        ~Game(); 

        void Run(); 
    private:
        void Init(); 
        void Loop(); 
        void Quit(); 

    private: 
        static constexpr float m_TargetFPS = 60.f;
        static constexpr bool m_RenderGraphics = false; 
        ECS::Scene m_Arena;  
        std::size_t m_SceneIndex;
        GraphicsSystem* m_GraphicsSystem; 
        PhysicsSystem* m_PhysicsSystem; 
        ServerEventSystem* m_ServerEventSystem; 
    }; 

}

#endif 