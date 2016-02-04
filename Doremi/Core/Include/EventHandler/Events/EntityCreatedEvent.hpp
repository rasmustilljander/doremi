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
        struct EntityCreatedEvent : public Event
        {
            EntityCreatedEvent(size_t p_playerEntityID, Blueprints p_blueprint)
                : Event(EventType::EntityCreated), entityID(p_playerEntityID), bluepirnt(p_blueprint)
            {
            }
            size_t entityID;
            Blueprints bluepirnt;
        };
    }
}
