// entry point
#include "All.h"
#include "Window.h"
#include "Util.h"
#include "Texture.h"


auto main(int argc, char* argv[]) -> int {
	SDL_Init(SDL_INIT_EVERYTHING);
	Window window;
	if (!window.InitWindow("hello_window", 900, 600)) {
	}
	TextureManager::Get().AddTexture(window, "Hello", "foo.bmp");
	window.Clear();
	window.RenderTexture(TextureManager::Get()["Hello"], nullptr, nullptr);
	window.ShowBuffers();
	SDL_Quit();
	return EXIT_SUCCESS;
}
