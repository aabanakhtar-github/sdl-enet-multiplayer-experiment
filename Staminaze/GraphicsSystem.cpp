#include "GraphicsSystem.h"
#include "All.h"
#include "Components.h"

GraphicsSystem::GraphicsSystem()
	: m_Window(std::make_unique<Window>())
{
	if (m_Window->InitWindow("Hello World!", 1280, 720))
	{
		// application fail state, fail gracefully
	}	

	TextureManager::Get().AddTexture(*m_Window, "foo.bmp", "foo");
}

void GraphicsSystem::Update(ECS::Scene& scene, float delta, bool draw_debug_rect)
{ 
	ECS::SceneView<TextureComponent, PositionComponent> IDs(scene);
	TextureManager& manager = TextureManager::Get();
	
	m_Window->Clear();

	for (ECS::EntityID ID : IDs.Get())
	{
		TextureComponent& texture_ref = scene.GetComponent<TextureComponent>(ID);
		PositionComponent& position_ref = scene.GetComponent<PositionComponent>(ID);
		TextureData& texture = manager.GetTexture(texture_ref.TextureName);
		
		assert(texture_ref.Scale > 0.f && "Cannot create render image of scale <0!");

		Rect src = texture_ref.SourceRectangle;
		Rect destination{(int)position_ref.X, (int)position_ref.Y, (int)(float(texture.GetWidth()) * texture_ref.Scale), (int)(float(texture.GetHeight()) * texture_ref.Scale)};
		m_Window->RenderTexture(texture, &src, &destination);
		m_Window->DrawRect(destination);
	}	

	m_Window->ShowBuffers();
}