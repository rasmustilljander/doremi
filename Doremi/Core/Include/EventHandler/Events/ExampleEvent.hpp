#pragma once
#include <EventHandler/Events/Event.hpp>
/**
Only an example of what an event might look like
Not needed for project. Could be removed when examples are no longer needed*/

namespace Doremi
{
    namespace Core
    {
        struct ExampleEvent : public Event
        {
            ExampleEvent() { myInt = 0; }
            int myInt;
        };
    }
}
