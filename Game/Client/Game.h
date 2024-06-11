#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include <array>
#include "ECS.h"

class GraphicsSystem;
class PhysicsSystem;
class ClientEventSystem;
class AnimationSystem;

namespace Client {

    class Game {
    public: 
        explicit Game();
        ~Game(); 

        void Run(); 
    private:
        void init();
        void loop();
        void quit();

    private:
        static constexpr float target_fps_ = 60.0f;
        std::array<ECS::Scene, 1> game_scenes_;
        std::size_t scene_index_;
        GraphicsSystem* graphics_system_;
        ClientEventSystem* player_input_system_;
        AnimationSystem* animation_system_;
    };

}

#endif