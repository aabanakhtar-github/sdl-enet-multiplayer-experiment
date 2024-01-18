#include "All.h" 

class Game
{
	MAKE_SINGLETON(Game)
public:
	Game();
	~Game()
	{
		ShutDownCore();
	}

	bool Init(int argc, char* argv[]);
	void Run();

private:
	ECS::Scene m_GameScene;
	ECS::EntityID m_Picture;
};