#pragma once
#include <EventHandler/Events/Event.hpp>
/**
Only an example of what an event might look like
Not needed for project. Could be removed when examples are no longer needed*/

namespace Doremi
{
    namespace Core
    {
        struct PlayerCreationEvent : public Event
        {
            PlayerCreationEvent(int p_playerEntityID) : Event(EventType::PlayerCreation), playerEntityID(p_playerEntityID) {}
            PlayerCreationEvent() : Event(EventType::PlayerCreation) {}
            int playerEntityID;
        };
    }
}
