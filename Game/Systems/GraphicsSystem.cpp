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
	auto protected_load = [this](const std::string& file, const std::string& name) -> void {
		bool success = TextureManager::Get().AddTexture(*GameWindow, file, name);
		if (!success) 
		{
			// use a placeholder
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
		
		assert(texture_ref.Scale > 0.f && "Cannot create render image of scale <0!");

		Rect src = texture_ref.SourceRectangle;
		Rect destination { (int)position_ref.X, (int)position_ref.Y, (int)(float(texture.GetWidth()) * texture_ref.Scale), (int)(float(texture.GetHeight()) * texture_ref.Scale) };
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
