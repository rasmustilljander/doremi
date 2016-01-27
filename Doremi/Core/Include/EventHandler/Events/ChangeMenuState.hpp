#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <Doremi/Core/Include/Helper/DoremiStates.hpp>

/**
Used for changing the state of the game
*/

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
