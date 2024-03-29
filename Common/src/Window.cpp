#include "Window.h" 
#include <format>
#include "Util.h "
#include "Texture.h"
#include "AppState.h"

Window::Window() 
	: m_SDLRenderer(nullptr), m_SDLWindow(nullptr)
{	
}

bool Window::InitWindow(const std::string& name, std::uint16_t w, std::uint16_t h)
{
	m_SDLWindow = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, (int)w, (int)h, SDL_WINDOW_SHOWN );
	if (m_SDLWindow == nullptr)
	{
		GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Couldn't create window! SDL_ERROR: " + std::string(SDL_GetError()));
		return false;	
	}
	
	m_SDLRenderer = SDL_CreateRenderer(m_SDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (m_SDLRenderer == nullptr)
	{
		GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Couldn't create renderer! SDL_ERROR: " + std::string(SDL_GetError()));
		return false;	
	}
	
	return true;
}

Window::~Window() 
{
	SDL_DestroyWindow(m_SDLWindow);
	SDL_DestroyRenderer(m_SDLRenderer);
	m_SDLWindow = nullptr; 
	m_SDLRenderer = nullptr; 
} 

void Window::RenderTexture(TextureData& tex, Rect* src, Rect* dst)
{
	SDL_RenderCopy(m_SDLRenderer, tex.Texture, src, dst);
}

void Window::DrawRect(Rect& rect)
{
	std::uint8_t r, g, b, a;
	SDL_GetRenderDrawColor(m_SDLRenderer, &r, &g, &b, &a);
	SDL_SetRenderDrawColor(m_SDLRenderer, 255, 0, 0, 0);
	SDL_RenderDrawRect(m_SDLRenderer, &rect);
	SDL_SetRenderDrawColor(m_SDLRenderer, r, g, b, a);
}