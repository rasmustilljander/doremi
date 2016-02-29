#pragma once
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <EventHandler/Events/Event.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TriggerComponent.hpp>
/**
Only an example of what an event might look like
Not needed for project. Could be removed when examples are no longer needed*/

namespace Doremi
{
    namespace Core
    {
        struct TriggerEvent : public Event
        {
            TriggerEvent() : Event(EventType::Trigger), triggerEntityID(0), objectEntityID(0), triggerType(TriggerType::NoTrigger) {}

            TriggerEvent(TriggerType p_triggerType, EntityID p_objectEntityID, EntityID p_triggerEntityID)
                : Event(EventType::Trigger), objectEntityID(p_objectEntityID), triggerEntityID(p_triggerEntityID), triggerType(p_triggerType)
            {
            }

            /**
                Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override
            {
                p_streamer->WriteUnsignedInt32(triggerEntityID);
                p_streamer->WriteUnsignedInt32(objectEntityID);
                p_streamer->WriteUnsignedInt8((uint8_t)triggerType);
                op_bitsWritten += sizeof(uint32_t) * 2 * 8 + sizeof(uint8_t) * 8;
            }

            /**
                Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override
            {
                triggerEntityID = p_streamer->ReadUnsignedInt32();
                objectEntityID = p_streamer->ReadUnsignedInt32();
                triggerType = (TriggerType)p_streamer->ReadUnsignedInt8();
                op_bitsRead += sizeof(uint32_t) * 2 * 8 + sizeof(uint8_t) * 8;
            }

            EntityID triggerEntityID;
            EntityID objectEntityID;
            TriggerType triggerType;
        };
    }
}
