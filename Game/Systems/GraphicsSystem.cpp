#include "GraphicsSystem.h"
#include "Components.h"
#include "Texture.h"
#include "Window.h"
#include "AppState.h"

#include <memory>

GraphicsSystem::GraphicsSystem()
	: GameWindow(std::move(std::make_unique<Window>()))
{
	if (GameWindow->InitWindow("Hello World!", 1280, 720))
	{
		GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Couldn't open window!");
	}	
}

void GraphicsSystem::InitMedia()
{
	std::vector<bool> failures;
	failures.push_back(TextureManager::Get().AddTexture(*GameWindow, "foo.bmp", "foo"));
	
	if (std::find(failures.begin(), failures.end(), false) != failures.end())
	{
		GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Cannot load textures! Check Media.");
	}
}

void GraphicsSystem::Update(ECS::Scene& scene, float delta, bool draw_debug_rect)
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

		// TODO: fix, getwidth returns size of original texture
		Rect src = texture_ref.SourceRectangle;
		Rect destination { (int)position_ref.X, (int)position_ref.Y, (int)(float(texture.GetWidth()) * texture_ref.Scale), (int)(float(texture.GetHeight()) * texture_ref.Scale) };
		GameWindow->RenderTexture(texture, &src, &destination);
		GameWindow->DrawRect(destination);
	}	

	for (ECS::EntityID ID : PhysicsIDs.GetEntities())
	{
		TextureComponent& texture_ref = scene.GetComponent<TextureComponent>(ID);
		PhysicsBodyComponent& physics_ref = scene.GetComponent<PhysicsBodyComponent>(ID);
		TextureData& texture = manager.GetTexture(texture_ref.TextureName);
		
		assert(texture_ref.Scale > 0.f && "Cannot create render image of scale <0!");

		Rect src = texture_ref.SourceRectangle;
		Rect destination{ physics_ref.BoundingBox.x, physics_ref.BoundingBox.y, physics_ref.BoundingBox.w, physics_ref.BoundingBox.h };
		std::cout << destination.y << std::endl;
		GameWindow->RenderTexture(texture, &src, &destination);
		GameWindow->DrawRect(destination);
	}

	GameWindow->ShowBuffers();
}
