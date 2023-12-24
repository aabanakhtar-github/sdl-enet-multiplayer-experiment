#ifndef WINDOW_H
#define WINDOW_H

#include "All.h" 

class Window 
{
public: 
	Window(); 
	~Window(); 
	
	bool InitWindow(const std::string& name, std::uint16_t w, std::uint16_t h);
	
private:
	SDL_Window* m_SDLWindow;
	SDL_Renderer* m_SDLRenderer;
};

#endif