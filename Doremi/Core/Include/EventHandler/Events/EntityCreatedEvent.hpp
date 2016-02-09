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
                : Event(EventType::EntityCreated), entityID(p_playerEntityID), bluepirnt(p_blueprint), position(DirectX::XMFLOAT3(0, 0, 0))
            {
            }
            EntityCreatedEvent(size_t p_playerEntityID, Blueprints p_blueprint, DirectX::XMFLOAT3 p_position)
                : Event(EventType::EntityCreated), entityID(p_playerEntityID), bluepirnt(p_blueprint), position(p_position)
            {
            }

            /**
                Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override
            {
                p_streamer->WriteUnsignedInt32((uint32_t)bluepirnt);
                op_bitsWritten += sizeof(uint32_t) * 8;

                p_streamer->WriteFloat3(position);
                op_bitsWritten += sizeof(float) * 4 * 8;
            }

            /**
                Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override
            {
                bluepirnt = (Blueprints)p_streamer->ReadUnsignedInt32();
                op_bitsRead += sizeof(uint32_t) * 8;

                position = p_streamer->ReadFloat3();
                op_bitsRead += sizeof(float) * 4 * 8;
            }

            size_t entityID;
            Blueprints bluepirnt;
            DirectX::XMFLOAT3 position;
        };
    }
}
