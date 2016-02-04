#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <stdint.h>

namespace Doremi
{
    namespace Core
    {
        struct RemoveEntityEvent : public Event
        {
            RemoveEntityEvent() : Event(EventType::RemoveEntity) {}
            RemoveEntityEvent(uint32_t p_entityID) : Event(EventType::RemoveEntity), entityID(p_entityID) {}

            uint32_t entityID;
        };
    }
}