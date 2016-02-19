#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <stdint.h>


namespace Doremi
{
    namespace Core
    {
        struct SetHealthEvent : public Event
        {
            SetHealthEvent() : Event(EventType::SetHealth) {}
            SetHealthEvent(const uint32_t& p_entityID, const float& p_health) : Event(EventType::SetHealth), entityID(p_entityID), health(p_health) {}

            /**
            Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override
            {
                p_streamer->WriteUnsignedInt32(entityID);
                p_streamer->WriteFloat(health);
                op_bitsWritten += (sizeof(uint32_t) + sizeof(float)) * 8;
            }

            /**
            Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override
            {
                entityID = p_streamer->ReadUnsignedInt32();
                health = p_streamer->ReadFloat();
                op_bitsRead += (sizeof(uint32_t) + sizeof(float)) * 8;
            }

            EntityID entityID;
            float health;
        };
    }
}