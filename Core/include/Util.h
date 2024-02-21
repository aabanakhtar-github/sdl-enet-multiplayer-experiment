#ifndef UTIL_H
#define UTIL_H

#include "SDL.h"
#include "Components.h"
#include <iostream>
#include <cassert>
#include <string>
#include <format>

#define MAKE_SINGLETON(classname) \
	public: \
		static classname& Get() { \
			static classname s_instance; \
			return s_instance; \
	} \
	private: \
			classname& operator = (const classname&) = delete; \
			classname(const classname&) = delete; \

inline void InitCore()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
	}
}

inline void ShutDownCore()
{
	SDL_Quit();
}

#endif //UTIL_H