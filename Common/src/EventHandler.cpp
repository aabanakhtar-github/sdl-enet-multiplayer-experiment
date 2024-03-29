#include "EventHandler.h"

void EventHandler::BindEvent(SDL_EventType event, std::function<void(SDL_Event&)> binding)
{
	if (m_Bindings.contains(event))
	{
		m_Bindings[event].push_back(binding);
	} 
	else
	{
		m_Bindings[event] = { binding };
	}
}


UserEventCreationInfo EventHandler::AddSDLUserEvent()
{
	std::uint32_t EventID = SDL_RegisterEvents(1);
	if (EventID == (std::uint32_t)-1)
	{
		return UserEventCreationInfo();
	}
	return UserEventCreationInfo{ true, EventID };
}

void EventHandler::PushSDLEvent(SDL_Event& event)
{
	SDL_PushEvent(&event);
}

void EventHandler::PushUserEvent(SDL_EventType event, void* data1, void* data2)
{
	SDL_Event user_event;
	user_event.type = event; 
	user_event.user.data1 = data1;
	user_event.user.data2 = data2;
	SDL_PushEvent(&user_event);
}

void EventHandler::Update()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (m_Bindings.contains((SDL_EventType)event.type))
		{
			auto bindings = m_Bindings[(SDL_EventType)event.type];
			for (auto &func : bindings)
			{
				func(event);
			}
		}
	}
}