// entry point
#include "Game.h"

int main(int argc, char* argv[])
{
	Game& game = Game::Get();
	game.Init(argc, argv);
	game.Run();
	game.Close();
	return 0;
}
