#include "Window.h" 
#include <format>
#include <cassert>
#include "Util.h "
#include "Texture.h"

Window::Window() 
	: m_SDLRenderer(nullptr), m_SDLWindow(nullptr) {
}

bool Window::init(const std::string& name, int w, int h) {
    assert((w > 0 && h > 0) && "The window should be greater than 0 in any dimension");
    m_SDLWindow = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);

    if (m_SDLWindow == nullptr) {
		GlobalAppState::get().setAppState(AppState::AS_FAIL, "Couldn't create window! SDL_ERROR: " + std::string(SDL_GetError()));
		return false;	
	}
	
	m_SDLRenderer = SDL_CreateRenderer(m_SDLWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (m_SDLRenderer == nullptr) {
		GlobalAppState::get().setAppState(AppState::AS_FAIL, "Couldn't create renderer! SDL_ERROR: " + std::string(SDL_GetError()));
		return false;	
	}
	
	return true;
}

Window::~Window() {
    if (m_SDLWindow != nullptr) {
        SDL_DestroyWindow(m_SDLWindow);
    }

    if (m_SDLRenderer != nullptr) {
        SDL_DestroyRenderer(m_SDLRenderer);
    }

	m_SDLWindow = nullptr;
	m_SDLRenderer = nullptr; 
} 

void Window::renderTexture(TextureData& tex, Rect* src, Rect* dst) {
    if (tex.Texture != nullptr) {
        SDL_RenderCopy(m_SDLRenderer, tex.Texture, src, dst);
    }
}

void Window::drawRect(Rect& rect) {
	std::uint8_t r, g, b, a;
	SDL_GetRenderDrawColor(m_SDLRenderer, &r, &g, &b, &a);
	SDL_SetRenderDrawColor(m_SDLRenderer, 255, 0, 0, 0);
	SDL_RenderDrawRect(m_SDLRenderer, &rect);
	SDL_SetRenderDrawColor(m_SDLRenderer, r, g, b, a);
}