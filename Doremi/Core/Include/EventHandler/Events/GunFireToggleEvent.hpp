#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <stdint.h>


namespace Doremi
{
    namespace Core
    {
        struct GunFireToggleEvent : public Event
        {
            GunFireToggleEvent() : Event(EventType::RemoveEntity) {}
            GunFireToggleEvent(uint32_t p_entityID, bool p_isFiring) : Event(EventType::GunFireToggle), entityID(p_entityID), isFiring(p_isFiring) {}

            /**
            Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override
            {
                p_streamer->WriteUnsignedInt32(entityID);
                p_streamer->WriteBool(isFiring);
                op_bitsWritten += sizeof(uint32_t) * 8 + 1;
            }

            /**
            Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override
            {
                entityID = p_streamer->ReadUnsignedInt32();
                isFiring = p_streamer->ReadBool();
                op_bitsRead += sizeof(uint32_t) * 8 + 1;
            }

            EntityID entityID;
            bool isFiring;
        };
    }
}