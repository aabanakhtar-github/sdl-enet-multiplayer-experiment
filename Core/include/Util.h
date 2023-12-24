#ifndef UTIL_H
#define UTIL_H

#include "SDL.h"
#include <iostream>

inline void PRINT_SDL_ERROR_MESSAGE(const std::string& user_data)
{
#ifdef __DEBUG__ 
	std::cout << "ERROR! " << user_data << "; SDL_Error: " << SDL_GetError() << "..." << std::endl;
#endif
}

#endif