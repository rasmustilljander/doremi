#pragma once
#include <EventHandler/Events/Event.hpp>
/**
Only an example of what an event might look like
Not needed for project. Could be removed when examples are no longer needed*/

namespace Doremi
{
    namespace Core
    {
        struct TriggerManagerEventStruct : public Event
        {
            TriggerManagerEventStruct() : Event(EventType::TriggerManagerEventTest) { myInt = 0, duOxo = 0; }
            int myInt;
            int duOxo;
        };
    }
}
