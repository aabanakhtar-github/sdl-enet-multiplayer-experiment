#include "Game.h"
#include "SDL.h" // SDL linker error prevention

// the one and only, the holy grail, the main function
int main(int argc, char *argv[]) {
  Client::Game game;
  game.Run();
  return 0;
}
