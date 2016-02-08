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

            /**
                Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override {}

            /**
                Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override {}

            uint32_t entityID;
        };
    }
}