#include "Components.h" 
#include "Util.h" 
#include "PhysicsSystem.h"
#include "ServerEventSystem.h"
#include <iostream>

void PhysicsSystem::update(ECS::Scene &scene, float delta) {
	ECS::SceneView<PhysicsBodyComponent> physics_bodies(scene);

	for (auto ID : physics_bodies.getEntities()) {
		auto& component = scene.getComponent<PhysicsBodyComponent>(ID);

        if (component.SimulatesPhysics) {
			component.Acceleration.y += gravity_;
		}	

		component.Velocity += component.Acceleration;
        component.BoundingBox.x += static_cast<int>(component.Velocity.x * delta);

		for (auto other : physics_bodies.getEntities()) {
			if (ID == other) continue;

			auto& other_component = scene.getComponent<PhysicsBodyComponent>(other);

			if (isColliding(other_component.BoundingBox, component.BoundingBox)) {
				if (component.Velocity.x > 0) {
					component.BoundingBox.x = other_component.BoundingBox.x - component.BoundingBox.w;
				} else if (component.Velocity.x < 0) {
					component.BoundingBox.x = other_component.BoundingBox.x + other_component.BoundingBox.w;
				}

				component.Velocity.x = 0;
			}
		}	
		
		component.BoundingBox.y += static_cast<int>(component.Velocity.y * delta);
        component.grounded = false;

        for (auto other : physics_bodies.getEntities()) {
			if (ID == other) continue;

			auto& other_component = scene.getComponent<PhysicsBodyComponent>(other);

            if (isColliding(other_component.BoundingBox, component.BoundingBox)) {
				if (component.Velocity.y > 0) {
					component.BoundingBox.y = other_component.BoundingBox.y - component.BoundingBox.h;
                    component.grounded = true;
				} else if (component.Velocity.y < 0) {
					component.BoundingBox.y = other_component.BoundingBox.y + other_component.BoundingBox.h;
				}

				component.Velocity.y = 0;
			}
		}		

		component.Acceleration = { 0, 0 };
	}

    auto event_system = ECS::SystemManager::get().getSystem<ServerEventSystem>();
    const auto& players = event_system->getClientToECS_IDArray();

    for (auto& i : event_system->anim_states) {
        i = "idle";
    }

    for (std::size_t i = 0; i < players.size(); ++i) {
        if (!scene.entityActive(players[i])) {
            continue;
        }

        auto& component = scene.getComponent<PhysicsBodyComponent>(players[i]);
        if (component.Velocity.y != 0) {
           event_system->anim_states[i] = "jump";
        } else {
            event_system->anim_states[i] = "idle";
        }
    }
}

void PhysicsSystem::init(ECS::Scene &scene) {

}

