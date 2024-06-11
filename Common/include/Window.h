#ifndef WINDOW_H
#define WINDOW_H

#include <string> 
#include <cstdint>

#include "SDL.h"

using Rect = SDL_Rect;
class TextureData;

class Window {
public: 
	explicit Window(); 
	~Window();
    Window(const Window&) = delete;
    Window& operator = (const Window&) = delete;

    bool init(const std::string& name, int w, int h);
	// src == nullptr for whole tex, dst == nullptr for rendering to whole window
	void renderTexture(TextureData& tex, Rect* src, Rect* dst);
	void drawRect(Rect& r);
	void clear() { SDL_RenderClear(m_SDLRenderer);	}
	void showBuffers() { SDL_RenderPresent(m_SDLRenderer);	}

private:
	SDL_Window* m_SDLWindow;
	SDL_Renderer* m_SDLRenderer;
	
	friend class TextureData;
	friend class TextureManager;
};

#endif // WINDOW_H