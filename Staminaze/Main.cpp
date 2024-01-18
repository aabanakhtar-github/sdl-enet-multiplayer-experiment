// entry point
#include "Game.h"

int main(int argc, char* argv[])
{
	Game::Get().Init(argc, argv);
	Game::Get().Run();
	ShutDownCore();
	return 0;
}
