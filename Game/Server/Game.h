#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "ECS.h"

class GraphicsSystem;
class PhysicsSystem;
class AnimationSystem;
class ServerEventSystem;

namespace Server {

    class Game {
    public: 
        explicit Game();
        ~Game(); 

        void Run(); 
    private:
        void Init(); 
        void Loop();
        void Quit(); 

    private: 
        static constexpr float target_fps_ = 360.f;
        ECS::Scene arena_;
        GraphicsSystem* graphics_system_;
        PhysicsSystem* physics_system_;
        ServerEventSystem* server_event_system_;
        AnimationSystem* animation_system_;
    }; 

}

#endif 