// entry point
#include "All.h"
#include "Window.h"
#include "Util.h"

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING);
	Window window;
	if (!window.InitWindow("hello_window", 900, 600)) {
	}
	SDL_Quit();
	return EXIT_SUCCESS;
}
