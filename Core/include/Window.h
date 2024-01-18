#ifndef WINDOW_H
#define WINDOW_H

#include <string> 
#include <cstdint>

#include "SDL.h"

using Rect = SDL_Rect;
class TextureData;

class Window 
{
public: 
	explicit Window(); 
	~Window(); 
	
	bool InitWindow(const std::string& name, std::uint16_t w, std::uint16_t h);
	// src == nullptr for whole tex, dst == nullptr for rendering to whole window
	void RenderTexture(TextureData& tex, Rect* src, Rect* dst);
	void DrawRect(Rect& r);
	void Clear() { SDL_RenderClear(m_SDLRenderer);	}
	void ShowBuffers() { SDL_RenderPresent(m_SDLRenderer);	}

private:
	SDL_Window* m_SDLWindow;
	SDL_Renderer* m_SDLRenderer;
	
	friend class TextureData;
	friend class TextureManager;

private:
	Window(const Window&) = delete;
	Window& operator = (const Window&) = delete;
};

#endif