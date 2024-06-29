#include "Game.h"
#include "Netcode/NetUtil.h"
#include <iostream> 
#include "SDL.h" 

// the one and only, the holy grail, the main function
int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Incorrect Argument Count! usage: \"./Server <port>" << std::endl; 
    return 1; 
  }
  
  std::string port = argv[1];
  std::cout << "Attempting to create a server on port: " << port << std::endl;

  if (!checkPort(port)) { 
    std::cerr << "Invalid port, check your input!" << std::endl;  
    return 1; 
  }

  Server::Game game(std::stoi(port));
  game.run();
  return 0;
}


