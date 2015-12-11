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
            int playerEntityID;
            PlayerCreationEvent(int p_playerEntityID) : playerEntityID(p_playerEntityID) {}
            PlayerCreationEvent() {}
        };
    }
}
