#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
#include <Doremi/Core/Include/Helper/DoremiStates.hpp>

/**
Used for changing the state of the game
*/

namespace Doremi
{
    namespace Core
    {
        struct ChangeMenuState : public Event
        {
            ChangeMenuState() : Event(EventType::ChangeMenuState) {}

            /**
                Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override {}

            /**
                Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override {}

            DoremiStates state;
        };
    }
}
