#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
namespace Doremi
{
    namespace Core
    {
        /**
        Contains info on damage taken, make sure not to send to much of these since they are sent over network
        */
        struct DamageTakenEvent : public Event
        {
            EntityID entityId;
            float damage;

            DamageTakenEvent(const float& p_damage, const EntityID& p_entityId)
                : damage(p_damage), entityId(p_entityId), Event(EventType::DamageTaken)
            {
            }
            DamageTakenEvent() : entityId(0), damage(0), Event(EventType::DamageTaken) {}

            /**
                Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override
            {
                p_streamer->WriteUnsignedInt32(entityId);
                p_streamer->WriteFloat(damage);
                op_bitsWritten += sizeof(EntityID) * 8 + sizeof(float) * 8;
            }

            /**
                Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override
            {
                entityId = p_streamer->ReadUnsignedInt32();
                damage = p_streamer->ReadFloat();
                op_bitsRead += sizeof(EntityID) * 8 + sizeof(float) * 8;
            }
        };
    }
}