#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <Doremi/Core/Include/Helper/DoremiStates.hpp>

/**
Only an example of what an event might look like
Not needed for project. Could be removed when examples are no longer needed*/

namespace Doremi
{
    namespace Core
    {
        struct ChangeMenuState : public Event
        {
            ChangeMenuState() : Event(EventType::ChangeMenuState) {}
            DoremiStates state;
        };
    }
}
