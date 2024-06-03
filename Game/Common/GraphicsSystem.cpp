#include "GraphicsSystem.h"
#include "Components.h"
#include "Texture.h"
#include "Window.h"
#include "AppState.h"

#include <memory>
#include <algorithm>

GraphicsSystem::GraphicsSystem()
	: DrawDebugRects(false), GameWindow(std::move(std::make_unique<Window>()))
{
	if (!GameWindow->InitWindow("Hello World!", 1280, 720))
	{
		GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Couldn't open window!");
	}	
}

void GraphicsSystem::Init(ECS::Scene& scene)
{
	auto protected_load = [this](const std::string& file, const std::string& name) -> void 
	{
		bool success = TextureManager::Get().AddTexture(*GameWindow, file, name);
		if (!success) 
		{
			std::cerr << "Failed to load texture " << file << " with SDL_ERROR: " << SDL_GetError() << std::endl;
		}		
	};

	protected_load("foo.bmp", "foo");
}

void GraphicsSystem::Update(ECS::Scene& scene, float delta)
{ 
	ECS::SceneView<TextureComponent, PositionComponent> IDs(scene);
	ECS::SceneView<TextureComponent, PhysicsBodyComponent> PhysicsIDs(scene);

	TextureManager& manager = TextureManager::Get();
	
	GameWindow->Clear();

	for (ECS::EntityID ID : IDs.GetEntities())
	{
		TextureComponent& texture_ref = scene.GetComponent<TextureComponent>(ID);
		PositionComponent& position_ref = scene.GetComponent<PositionComponent>(ID);
		TextureData& texture = manager.GetTexture(texture_ref.TextureName);

		if (!texture.GetValid())
		{
			std::cerr << "Cannot render texture named " << texture_ref.TextureName << "! SDL_Error: " << SDL_GetError() << std::endl;
			continue;	
		}	

		assert((texture_ref.Scale.w > 0 && texture_ref.Scale.h > 0) && "Cannot create render image of scale <0!");

		Rect src = texture_ref.SourceRectangle;
		Rect destination { (int)position_ref.Position.X, (int)position_ref.Position.Y, texture_ref.Scale.w, texture_ref.Scale.h };
		GameWindow->RenderTexture(texture, &src, &destination);
	
		if (DrawDebugRects)	
		{
			GameWindow->DrawRect(destination);
		}
	}	

    // physics entities don't have postion component, handle seperate
	for (ECS::EntityID ID : PhysicsIDs.GetEntities())
	{
		TextureComponent& texture_ref = scene.GetComponent<TextureComponent>(ID);
		PhysicsBodyComponent& physics_ref = scene.GetComponent<PhysicsBodyComponent>(ID);
		TextureData& texture = manager.GetTexture(texture_ref.TextureName);

		if (!texture.GetValid())
		{
			std::cerr << "Cannot render texture named " << texture_ref.TextureName << "! SDL_Error: " << SDL_GetError() << std::endl;
			continue;	
		}

		Rect src = texture_ref.SourceRectangle;
		Rect destination{ physics_ref.BoundingBox.x, physics_ref.BoundingBox.y, physics_ref.BoundingBox.w, physics_ref.BoundingBox.h };
		GameWindow->RenderTexture(texture, &src, &destination);

		if (DrawDebugRects)
		{
			GameWindow->DrawRect(destination);
		}
	}

	GameWindow->ShowBuffers();
}
