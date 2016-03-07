#pragma once
#include <Doremi/Core/Include/Streamers/Serializable.hpp>

namespace Doremi
{
    namespace Core
    {
        /**
            Every new type of event needs to be inserted in this enum
            If you add, don't forget to add it to function InterpetEvent in NetworkEventReceiver
            Is not bitmask, so max eventypes are 255 for now
        */
        enum class EventType : uint8_t
        {
            Undefined,
            EntityCreated, // TODO This is currently Created on server, and ToCreate on client, might split in two and convert from one to other in
            // playerHandler
            RemoveEntity,
            Example, // Only an example
            PlayerCreation,
            RangedEnemyCreated,
            MeleeEnemyCreated,
            Trigger,
            ChangeMenuState,
            DamageTaken,
            PlayerRespawn, // TODO maybe fix name to more specific?
            GunFireToggle,
            PlaySound,
            SetHealth,
            SetTransform,
            AnimationTransition,
            LoadNewWorld,
        };

        /**All events needs to inherit this struct*/
        struct Event : public Serializable
        {
        public:
            explicit Event(EventType p_eventType) : eventType(p_eventType) {}
            /**
                Write object to stream
            */
            virtual void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) = 0;

            /**
                Read object from stream
            */
            virtual void Read(Streamer* p_streamer, uint32_t& op_bitsRead) = 0;

            // Since all events inherits this struct we can store the event type here
            EventType eventType;
        };
    }
}
