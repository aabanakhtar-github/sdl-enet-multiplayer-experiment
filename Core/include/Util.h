#ifndef UTIL_H
#define UTIL_H

#include "SDL.h"
#include "Components.h"
#include <iostream>
#include <cassert>
#include <string>
#include <format>

#ifdef __DEBUG__
#define CUSTOM_ASSERT(x, message) assert(x && message) 
#else
#define ASSERT(x)
#endif

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

inline bool IsColliding(Rect& a, Rect& b)
{
    if (a.x < b.x + b.w // is the left side before the right side of b? 
        && a.x + a.w > b.x 
        && a.y < b.y + b.h 
        && a.y + a.h > b.y// is the right side after the left side of b?
        )
    {
        return true;
    }
    return false;
}

#endif //UTIL_H