#include "Game.h"
#include "GraphicsSystem.h"
#include "EventHandler.h"
#include "Texture.h"

#include "Components.h"
#include "AppState.h"
#include "PhysicsSystem.h"

Game::Game()
{
	
}

bool Game::Init(int argc, char* argv[])
{
	InitCore();
	RegisterComponents(m_GameScene);
	InitMedia();
	BindEvents();
	return true;
}

void Game::InitMedia()
{
	GraphicsSystem::Get().InitMedia();
}

void Game::BindEvents()
{
	auto& eh = EventHandler::Get();
	eh.BindEvent(SDL_QUIT,
		[&](SDL_Event& e) -> void {
			GlobalAppState::Get().SetAppState(AppState::AS_QUIT);
		});
}

void Game::Run()
{
	ECS::EntityID e1 = m_GameScene.CreateEntity();
	m_GameScene.AddComponent<TextureComponent>(e1)
		.SetScale(1.0)
		.SetSourceRectangle(Rect(0, 0, 100, 100))
		.SetTextureName("foo");

	m_GameScene.AddComponent<PhysicsBodyComponent>(e1)
		.SetBoundingBox(Rect(0, 0, 100, 100))
		.SetPosition(Vector2(100, 50))
		.SetGravityEnabled(true)
		.SetSimulatesPhysics(true);

	bool application_is_running = true;
	GlobalAppState::Get().SetAppState(AppState::AS_LOOP);
	while (application_is_running)
	{
		switch (GlobalAppState::Get().GetAppState())
		{
		case AppState::AS_QUIT:
			application_is_running = false;
			break;

		case AppState::AS_LOOP:
			GraphicsSystem::Get().Update(m_GameScene, true);
			PhysicsSystem::Get().Update(m_GameScene, 0.0);
			EventHandler::Get().Update();
			break;
		
		case AppState::AS_FAIL:
			std::cout << "ERRORS have occured! Terminating ... " << std::endl;
			for (auto& error : GlobalAppState::Get().GetError())
			{
				std::cout << "ERROR: " << error << std::endl;
			}
			application_is_running = false;
			break;
		}
	}
}

void Game::Close()
{
	ShutDownCore();
}
