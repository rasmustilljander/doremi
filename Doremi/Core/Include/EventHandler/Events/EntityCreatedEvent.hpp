#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>

namespace Doremi
{
    namespace Core
    {
        /**
        COntains information on a entity id, the event type decides what subscribers that
        are interested in that particular entity.
        */
        struct EntityCreatedEvent : public Event
        {
            EntityCreatedEvent(size_t p_playerEntityID, EventType p_type) : Event(p_type), entityID(p_playerEntityID) {}
            EntityCreatedEvent() : Event(EventType::Undefined) {}
            size_t entityID;
        };
    }
}
