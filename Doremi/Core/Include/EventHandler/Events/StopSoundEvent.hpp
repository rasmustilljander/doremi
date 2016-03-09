#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <stdint.h>


namespace Doremi
{
    namespace Core
    {
        /**
        Stops a entitys sound if it's playing
        */
        struct StopSoundEvent : public Event
        {
            StopSoundEvent() : Event(EventType::Example), entityID(0), soundType(0) {}
            StopSoundEvent(uint32_t p_entityID, int32_t p_soundType) : Event(EventType::StopSound), entityID(p_entityID), soundType(p_soundType) {}

            /**
            Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override
            {
                p_streamer->WriteUnsignedInt32(entityID);
                p_streamer->WriteInt32(soundType);
                op_bitsWritten += sizeof(uint32_t) * 8 + sizeof(int32_t) * 8;
            }

            /**
            Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override
            {
                entityID = p_streamer->ReadUnsignedInt32();
                soundType = p_streamer->ReadInt32();
                op_bitsRead += sizeof(uint32_t) * 8 + sizeof(int32_t) * 8;
            }

            EntityID entityID;
            int32_t soundType;
        };
    }
}