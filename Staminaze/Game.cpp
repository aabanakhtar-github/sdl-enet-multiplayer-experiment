#include "Game.h"
#include "GraphicsSystem.h"
#include "Components.h"
#include "CollisionSystem.h"

Game::Game()
	: m_Picture(-1)
{

}

bool Game::Init(int argc, char* argv[])
{
	InitCore();
	return true;
}

void Game::Run()
{
	EventHandler handler;
	m_GameScene.RegisterComponent<PositionComponent>();
	m_GameScene.RegisterComponent<TextureComponent>();
	m_GameScene.RegisterComponent<AABBColliderComponent>();

	auto E1 = m_GameScene.CreateEntity();
	m_GameScene.AddComponent<PositionComponent>(E1).SetX(5).SetY(5);
	m_GameScene.AddComponent<TextureComponent>(E1).SetTextureName("foo").SetSourceRectangle({ 0,0,100,100 }).SetScale(1.0);
	m_GameScene.AddComponent<AABBColliderComponent>(E1).SetBoundingBox({ 5, 5, 100, 100 });

	auto E2 = m_GameScene.CreateEntity();
	m_GameScene.AddComponent<PositionComponent>(E2).SetX(5).SetY(5);
	m_GameScene.AddComponent<TextureComponent>(E2).SetTextureName("foo").SetSourceRectangle({ 0,0,100,100 }).SetScale(1.0);
	m_GameScene.AddComponent<AABBColliderComponent>(E2).SetBoundingBox({ 5, 5, 100, 100 });


	// TODO: Change :D
	handler.BindEvent(SDL_QUIT, [&](SDL_Event&)
		{
			exit(0);
		});

	handler.BindEvent(SDL_KEYDOWN, [&](SDL_Event& e)
		{
			if (e.key.keysym.sym == SDLK_d)
			{
				m_GameScene.GetComponent<PositionComponent>(E1).X += 5;
				m_GameScene.GetComponent<AABBColliderComponent>(E2).BoundingBox.x += 5;
			}
		});
	handler.BindEvent(SDL_KEYDOWN, [&](SDL_Event& e)
		{
			if (e.key.keysym.sym == SDLK_a)
			{
				m_GameScene.GetComponent<PositionComponent>(E1).X -= 5;
				m_GameScene.GetComponent<AABBColliderComponent>(E2).BoundingBox.x -= 5;
			}
		});

	GraphicsSystem& gfx = GraphicsSystem::Get();
	
	// game loop 
	while (true)
	{
		gfx.Update(m_GameScene);
		handler.Update();
		
		if (CollisionSystem::Get().IsColliding(m_GameScene.GetComponent<AABBColliderComponent>(E1), m_GameScene.GetComponent<AABBColliderComponent>(E2)))
		{
			std::cout << "hi!" << std::endl;
		}
	}

	ShutDownCore();
}