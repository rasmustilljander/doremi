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
            EntityCreatedEvent() : Event(EventType::Undefined) {}

            EntityCreatedEvent(size_t p_playerEntityID, Blueprints p_blueprint)
                : Event(EventType::EntityCreated), entityID(p_playerEntityID), bluepirnt(p_blueprint)
            {
            }

            /**
                Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override {}

            /**
                Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override {}

            size_t entityID;
            Blueprints bluepirnt;
        };
    }
}
