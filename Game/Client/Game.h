#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include <array>
#include "Netcode/Packet.h" 
#include "GraphicsSystem.h"
#include "PhysicsSystem.h"
#include "ClientEventSystem.h"
#include "Netcode/Client.h"
#include "ECS.h"

namespace Client 
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
        static constexpr float m_TargetFPS = 60.0f; 
        std::array<ECS::Scene, 1> m_GameScenes; 
        std::size_t m_SceneIndex;
        GraphicsSystem* m_GraphicsSystem; 
        PhysicsSystem* m_PhysicsSystem; 
        ClientEventSystem* m_PlayerInputSystem; 
    }; 

}

#endif