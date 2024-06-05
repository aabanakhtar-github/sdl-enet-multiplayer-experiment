#include "EventHandler.h"

void EventHandler::bindEvent(SDL_EventType event, const std::function<void(SDL_Event&)>& binding) {
	if (bindings_.contains(event)) {
		bindings_[event].push_back(binding);
	} else {
        bindings_[event] = {binding };
	}
}


UserEventCreationInfo EventHandler::addSDLUserEvent() {
	std::uint32_t EventID = SDL_RegisterEvents(1);
	if (EventID == (std::uint32_t) - 1) {
		return {};
	}

	return UserEventCreationInfo{ true, EventID };
}

void EventHandler::pushSDLEvent(SDL_Event& event) {
	SDL_PushEvent(&event);
}

void EventHandler::pushUserEvent(SDL_EventType event, void* data1, void* data2) {
	SDL_Event user_event;
	user_event.type = event; 
	user_event.user.data1 = data1;
	user_event.user.data2 = data2;
	SDL_PushEvent(&user_event);
}

void EventHandler::update() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (bindings_.contains((SDL_EventType)event.type)) {
			auto bindings = bindings_[(SDL_EventType)event.type];
			for (auto &func : bindings) {
				func(event);
			}
		}
	}
}

