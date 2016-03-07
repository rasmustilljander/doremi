#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <Doremi/Core/Include/EntityComponent/Constants.hpp>
#include <stdint.h>


namespace Doremi
{
    namespace Core
    {
        // RUN, IDLE, FIRE, HIT, DEATH, JUMP
        enum class Animation
        {
            STOPATTACK,
            ATTACK,
            HIT,
            DEATH,
            JUMP,
        };
        struct AnimationTransitionEvent : public Event
        {
            AnimationTransitionEvent() : Event(EventType::AnimationTransition), entityID(0), animation(Animation::STOPATTACK) {}
            AnimationTransitionEvent(uint32_t p_entityID, Animation p_animation)
                : Event(EventType::AnimationTransition), entityID(p_entityID), animation(p_animation)
            {
            }

            /**
            Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override
            {
                p_streamer->WriteUnsignedInt32(entityID);
                p_streamer->WriteUnsignedInt8(static_cast<uint8_t>(animation));
                op_bitsWritten += sizeof(uint32_t) * 8;
                op_bitsWritten += sizeof(uint8_t) * 8;
            }

            /**
            Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override
            {
                entityID = p_streamer->ReadUnsignedInt32();
                animation = static_cast<Animation>(p_streamer->ReadUnsignedInt8());
                op_bitsRead += sizeof(uint32_t) * 8;
                op_bitsRead += sizeof(uint8_t) * 8;
            }

            EntityID entityID;
            Animation animation;
        };
    }
}