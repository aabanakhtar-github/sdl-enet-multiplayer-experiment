#ifndef UTIL_H
#define UTIL_H

#include "SDL.h"
#include "SDL_image.h"
#include "enet/enet.h"
#include <iostream>
#include <cassert>
#include <string>
#include <format>
#include <functional> 
#include <vector> 

#define MAKE_SINGLETON(classname) \
	public: \
		static classname& Get() { \
			static classname s_instance; \
			return s_instance; \
	} \
	private: \
			classname& operator = (const classname&) = delete; \
			classname(const classname&) = delete; \

enum class AppState
{
	AS_FAIL, 
	AS_QUIT,
	AS_LOOP
};

class GlobalAppState
{
	MAKE_SINGLETON(GlobalAppState)

	GlobalAppState() : m_AppState(AppState::AS_LOOP), m_Error() {}
public:
	void SetAppState(AppState state, const std::string& error = "")
	{
		m_AppState = state;
		if (error.size() > 0)
		{
			m_Error.push_back(error);
		}
	}

	AppState GetAppState() const {	return m_AppState; }
	const std::vector<std::string>& GetError() const { return m_Error;}
private:
	AppState m_AppState;
	std::vector<std::string> m_Error;
};

inline void InitializeLibraries()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Cannot initalize SDL!");
	}

	if (IMG_Init(IMG_INIT_PNG) != 0) 
	{
		GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Cannot initalize SDL_image!"); 
	}

	if (enet_initialize() < 0) 
	{
		GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Cannot initalize enet!"); 
	}

	GlobalAppState::Get().SetAppState(AppState::AS_LOOP); 
}

inline void ShutdownLibraries()
{
	SDL_Quit();
	enet_deinitialize(); 
}

#endif //UTIL_H