#include "Window.h" 
#include "Util.h "
#include "Texture.h"

Window::Window() 
	: m_SDLRenderer(nullptr), m_SDLWindow(nullptr)
{	
}

bool Window::InitWindow(const std::string& name, std::uint16_t w, std::uint16_t h)
{
	m_SDLWindow = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int)w, (int)h, SDL_WINDOW_SHOWN );
	if (m_SDLWindow == nullptr)
	{
		PRINT_SDL_ERROR_MESSAGE("Could not create Window!");
		return false;
	}
	m_SDLRenderer = SDL_CreateRenderer(m_SDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (m_SDLRenderer == nullptr) 
	{
		PRINT_SDL_ERROR_MESSAGE("Could not create Renderer!");
		return false;
	}
	return true;
}

Window::~Window() 
{
	if (m_SDLWindow != nullptr) 
	{
		SDL_DestroyWindow(m_SDLWindow);
	}
	if (m_SDLRenderer != nullptr)
	{
		SDL_DestroyRenderer(m_SDLRenderer);
	}
} 

void Window::RenderTexture(TextureData& tex, Rect* src, Rect* dst)
{
	SDL_RenderCopy(m_SDLRenderer, tex.Texture, src, dst);
}