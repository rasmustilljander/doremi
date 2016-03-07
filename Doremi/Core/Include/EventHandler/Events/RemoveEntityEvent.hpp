#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <stdint.h>

namespace Doremi
{
    namespace Core
    {
        struct RemoveEntityEvent : public Event
        {
            RemoveEntityEvent() : Event(EventType::RemoveEntity), entityID(0) {}
            RemoveEntityEvent(uint32_t p_entityID) : Event(EventType::RemoveEntity), entityID(p_entityID) {}

            /**
                Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override
            {
                p_streamer->WriteUnsignedInt32(entityID);
                op_bitsWritten += sizeof(uint32_t) * 8;
            }

            /**
                Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override
            {
                entityID = p_streamer->ReadUnsignedInt32();
                op_bitsRead += sizeof(uint32_t) * 8;
            }

            uint32_t entityID;
        };
    }
}