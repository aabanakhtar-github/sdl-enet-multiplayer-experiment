#include "GraphicsSystem.h"
#include "Components.h"
#include "Texture.h"
#include "Window.h"
#include <memory>
#include <algorithm>
#include <iostream>

GraphicsSystem::GraphicsSystem()
    : camera(),
      game_window_() {
    if (!game_window_.init("Multiplayer Demo", 900, 600)) {
		GlobalAppState::get().setAppState(AppState::AS_FAIL, "Couldn't open window!");
	}	
}

void GraphicsSystem::init(ECS::Scene &scene) {
	auto protected_load = [this](const std::string& file, const std::string& name) -> void {
		bool success = TextureManager::get().addTexture(game_window_, file, name);

        if (!success) {
			std::cerr << "Failed to load texture " << file << " with SDL_ERROR: " << SDL_GetError() << std::endl;
		}		
	};

	protected_load("Assets/tileset.png", "foo");
    protected_load("Assets/background.png", "bg");
    protected_load("Assets/player_idle.png", "player_idle");
    protected_load("Assets/player_run.png", "player_run");
    protected_load("Assets/player_attack.png", "player_attack");
}

void GraphicsSystem::update(ECS::Scene &scene, float delta) {
	ECS::SceneView<TextureComponent, PositionComponent> IDs(scene);
	ECS::SceneView<TextureComponent, PhysicsBodyComponent> PhysicsIDs(scene);
	TextureManager& manager = TextureManager::get();
	
    game_window_.clear();

    for (ECS::EntityID ID : IDs.getEntities()) {
		auto& texture_component = scene.getComponent<TextureComponent>(ID);
		auto& position_component = scene.getComponent<PositionComponent>(ID);
		TextureData& texture = manager.getTexture(texture_component.TextureName);

		if (!texture.getValid()) {
			std::cerr << "Cannot render texture named " << texture_component.TextureName << "! SDL_Error: " << SDL_GetError() << std::endl;
			continue;	
		}	

		assert((texture_component.Scale.w > 0 && texture_component.Scale.h > 0) && "Cannot create render image of scale <0!");

        Rect src = texture_component.SourceRectangle;
        Vector2 transformed = camera.transform({ position_component.Position.x, position_component.Position.y});
        FRect destination = { transformed.x, transformed.y, static_cast<float>(src.w), static_cast<float>(src.h) };

		game_window_.renderTexture(texture, &src, &destination, texture_component.flip);

		if constexpr (draw_debug_rects_) {
			game_window_.drawRect(destination);
		}
	}	

    // physics entities don't have position component, handle separately
	for (ECS::EntityID ID : PhysicsIDs.getEntities()) {
		auto& texture_component = scene.getComponent<TextureComponent>(ID);
		auto& physics_component = scene.getComponent<PhysicsBodyComponent>(ID);
		TextureData& texture = manager.getTexture(texture_component.TextureName);

		if (!texture.getValid()) {
			std::cerr << "Cannot render texture named " << texture_component.TextureName << "! SDL_Error: " << SDL_GetError() << std::endl;
			continue;	
		}

        Rect src = texture_component.SourceRectangle;
        Vector2 transformed = camera.transform({static_cast<float>(physics_component.AABB.x), static_cast<float>(physics_component.AABB.y) });
        FRect destination = { transformed.x, transformed.y, static_cast<float>(physics_component.AABB.w), static_cast<float>(physics_component.AABB.h) };

        if (scene.hasComponent<PlayerComponent>(ID)) {
            auto& player_component = scene.getComponent<PlayerComponent>(ID);
            game_window_.renderTexture(texture, &src, &destination, player_component.facing_left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
        } else {
           game_window_.renderTexture(texture, &src, &destination);

        }

		if constexpr (draw_debug_rects_) {
            game_window_.drawRect(destination);
		}
	}

	game_window_.showBuffers();
}
