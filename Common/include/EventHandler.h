#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotInitializedField"
#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H 

#include <unordered_map>
#include <functional>
#include <vector>
#include <cstdint>
#include "SDL.h"
#include "Util.h"

struct UserEventCreationInfo
{
	bool valid = false;
	std::uint32_t ID = (std::uint32_t)-1;
};

class EventHandler
{
	MAKE_SINGLETON(EventHandler)
public:
	EventHandler() = default;
	
	void update();
	void bindEvent(SDL_EventType event, const std::function<void(SDL_Event&)>& binding);
	
	// save this value to access the created event later	
	[[nodiscard]] static UserEventCreationInfo addSDLUserEvent();
	static void pushSDLEvent(SDL_Event& event);

	// SELF-WARNING
	// If pushing data, ownership will not be taken
	// You must free the data yourself
	static void pushUserEvent(SDL_EventType event, void* data1 = nullptr,  void* data2 = nullptr);
private:
	std::unordered_map<SDL_EventType, std::vector<std::function<void(SDL_Event&)>>> bindings_;
};

#endif // EVENT_HANDLER_H



#pragma clang diagnostic pop