#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <unordered_map>
#include "ECS.h"
#include "Window.h"
#include "MathFuncs.h"
#include "Timer.h"

//forward declare your components here
struct TextureComponent;
struct PositionComponent;
struct PhysicsBodyComponent;
struct AnimationStateMachineComponent;
struct PlayerComponent;

inline void registerComponents(ECS::Scene& scene) {
    scene.registerComponent<TextureComponent>();
    scene.registerComponent<PositionComponent>();
    scene.registerComponent<PhysicsBodyComponent>();
    scene.registerComponent<AnimationStateMachineComponent>();
    scene.registerComponent<PlayerComponent>();
}

struct TextureComponent : public ECS::ComponentBase
{
	Rect SourceRectangle = {};	
	std::string TextureName = "";
	Rect Scale = {}; // overrided by physics body bounding box, if present
};

struct PositionComponent : public ECS::ComponentBase
{
	Vector2 Position;
};

struct PhysicsBodyComponent : public ECS::ComponentBase
{
	Rect BoundingBox = {};
	Vector2 Velocity = {};
	Vector2 Acceleration = {};	
	bool SimulatesPhysics = false;
    bool grounded = false;
	
	friend bool operator == (const PhysicsBodyComponent& a, const PhysicsBodyComponent& b) {
		return a.BoundingBox == b.BoundingBox && a.Velocity == b.Velocity
			&& a.Acceleration == b.Acceleration && a.SimulatesPhysics == b.SimulatesPhysics;
	}
};

struct AnimationStateMachineComponent : public ECS::ComponentBase {
    struct Animation {
        std::size_t current_frame;
        std::vector<TextureComponent> frames;
        float seconds_per_frame = 0.5f;
    };

    Timer time_elapsed;
    std::string state = "idle";
    std::string last_state;
    // e.g
    // WALKING -> walk_1, walk_2
    std::unordered_map<std::string, Animation> state_to_animation_frames;
};

// shell component to diffrentiate players from non-players
struct PlayerComponent : ECS::ComponentBase {

};

#endif // COMPONENTS_H