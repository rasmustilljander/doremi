#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <stdint.h>

namespace Doremi
{
    namespace Core
    {
        /**
        COntains information on a entity id, the event type decides what subscribers that
        are interested in that particular entity.
        */
        struct SpecialEntityCreatedEvent : public Event
        {
            SpecialEntityCreatedEvent(size_t p_playerEntityID, EventType p_eventType) : Event(p_eventType), entityID(p_playerEntityID) {}
            size_t entityID;
        };
    }
}
