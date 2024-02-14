#include "Util.h" 
#include "ECS.h"

class Game
{
	MAKE_SINGLETON(Game)
public:
	explicit Game();

	bool Init(int argc, char* argv[]);
	void Run();
	void Close();
private:
	void InitMedia();
	void BindEvents();
private:		
	ECS::Scene m_GameScene;
	ECS::EntityID m_Picture;
	
};