#include "Game.h"
#include "GraphicsSystem.h"
#include "EventHandler.h"
#include "Texture.h"
#include "Components.h"
#include "AppState.h"
#include "PhysicsSystem.h"
#include "PlayerInputSystem.h"

Game::Game()
{
	
}

bool Game::Init(int argc, char* argv[])
{
	InitCore();
	RegisterComponents(m_GameScene);

	GraphicsSystem::Get().Init(m_GameScene);
	PlayerInputSystem::Get().Init(m_GameScene);
	PhysicsSystem::Get().Init(m_GameScene); 
	
	auto& eh = EventHandler::Get();
	eh.BindEvent(SDL_QUIT,
		[&](SDL_Event& e) -> void {
			GlobalAppState::Get().SetAppState(AppState::AS_QUIT);
		});

	return true;
}

void Game::InitMedia()
{
}

void Game::BindEvents()
{

}

void Game::Run()
{
	ECS::EntityID e1 = m_GameScene.CreateEntity();
	m_GameScene.AddComponent<TextureComponent>(e1)
		.SetScale(1.0)
		.SetSourceRectangle(Rect(0, 0, 10, 100))
		.SetTextureName("foo")
		.SetScale(1.3);

	m_GameScene.AddComponent<PhysicsBodyComponent>(e1)
		.SetBoundingBox(Rect(0, 0, 10, 100))
		.SetGravityEnabled(true)
		.SetSimulatesPhysics(true);

	ECS::EntityID e2 = m_GameScene.CreateEntity();
		m_GameScene.AddComponent<TextureComponent>(e2)
		.SetSourceRectangle(Rect(0, 0, 100, 140))
		.SetTextureName("foo");

	m_GameScene.AddComponent<PhysicsBodyComponent>(e2)
		.SetBoundingBox(Rect(0, 400, 900, 50))
		.SetSimulatesPhysics(true)
		.SetGravityEnabled(false);

	ECS::EntityID e3 = m_GameScene.CreateEntity();
	m_GameScene.AddComponent<TextureComponent>(e3)
		.SetSourceRectangle(Rect(0, 0, 100, 100))
		.SetTextureName("foo");

	m_GameScene.AddComponent<PhysicsBodyComponent>(e3)
		.SetBoundingBox(Rect(400, 300, 100, 100))
		.SetSimulatesPhysics(true)
		.SetGravityEnabled(false);

	bool application_is_running = true;
	while (application_is_running)
	{
		AppState state = GlobalAppState::Get().GetAppState();		
		switch (state)
		{
		case AppState::AS_QUIT:
			application_is_running = false;
			break;
		case AppState::AS_FAIL:
			std::cout << "ERRORS have occured! Terminating ... " << std::endl;
			
			for (auto& error : GlobalAppState::Get().GetError())
			{
				std::cout << "ERROR: " << error << std::endl;
			}
		
			application_is_running = false;
			break;
	
		case AppState::AS_LOOP:
			EventHandler::Get().Update();
			PlayerInputSystem::Get().Update(m_GameScene, 0.0);	
			PhysicsSystem::Get().Update(m_GameScene, 0.0);
			GraphicsSystem::Get().Update(m_GameScene, 0.0);	
			break;
		}

		// in the event of an error
		if (!application_is_running) break;
	}
}

void Game::Close()
{
	ShutDownCore();
}
