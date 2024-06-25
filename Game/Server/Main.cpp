#include "Components.h"
#include "ECS.h"
#include "Game.h"
#include "SDL.h" // SDL_main goes crazy

int main(int argc, char *argv[]) {
  Server::Game game;
  game.Run();
  return 0;
}
