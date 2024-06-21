#ifndef WINDOW_H
#define WINDOW_H

#include <string> 
#include <cstdint>

#include "SDL.h"

using FRect = SDL_FRect;
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
	void renderTexture(TextureData& tex, Rect* src, SDL_FRect* dst, SDL_RendererFlip flip = SDL_FLIP_HORIZONTAL);
	void drawRect(FRect& r);
	void clear() { SDL_RenderClear(renderer_);	}
	void showBuffers() { SDL_RenderPresent(renderer_);	}

private:
	SDL_Window* window_;
	SDL_Renderer* renderer_;
	
	friend class TextureData;
	friend class TextureManager;
};

#endif // WINDOW_H
