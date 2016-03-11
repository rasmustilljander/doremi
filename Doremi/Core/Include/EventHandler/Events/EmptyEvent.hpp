#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
/**
Only an example of what an event might look like
Not needed for project. Could be removed when examples are no longer needed*/

namespace Doremi
{
    namespace Core
    {
        struct EmptyEvent : public Event
        {
            EmptyEvent() : Event(EventType::Undefined) {}
            explicit EmptyEvent(const EventType& p_eventType) : Event(p_eventType) {}
            /**
                Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override {}

            /**
                Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override {}
        };
    }
}
