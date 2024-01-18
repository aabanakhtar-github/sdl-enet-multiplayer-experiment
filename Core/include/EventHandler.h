#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H 

#include <unordered_map>
#include <functional>
#include <vector> 

#include "SDL.h"

#include "Util.h"

struct UserEventCreationInfo
{
	bool Valid = false;
	std::uint32_t ID = (std::uint32_t)-1;
};

class EventHandler
{
	MAKE_SINGLETON(EventHandler)
public:
	EventHandler() = default;
	
	void Update();
	
	// no way to init lambdas without passing by reference sadly
	void BindEvent(SDL_EventType event, std::function<void(SDL_Event&)> binding);
	
	// save this value to access the created event later	
	[[nodiscard]] UserEventCreationInfo AddSDLUserEvent();
	void PushSDLEvent(SDL_Event& event);

	// WARNING
	// If pushing data, ownership will not be taken
	// You must free the data yourself
	void PushUserEvent(SDL_EventType event, void* data1 = nullptr,  void* data2 = nullptr);
private:
	std::unordered_map<SDL_EventType, std::vector<std::function<void(SDL_Event&)>>> m_Bindings;
};

#endif // EVENT_HANDLER_H


