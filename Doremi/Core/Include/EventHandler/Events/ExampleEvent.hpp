#pragma once
#include <Doremi/Core/Include/EventHandler/Events/Event.hpp>
/**
Only an example of what an event might look like
Not needed for project. Could be removed when examples are no longer needed*/

namespace Doremi
{
    namespace Core
    {
        struct ExampleEvent : public Event
        {
            ExampleEvent() : Event(EventType::Example) { myInt = 0; }

            /**
                Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override
            {
                p_streamer->WriteBool(true);
                op_bitsWritten += 1;
            }

            /**
                Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override
            {
                p_streamer->ReadBool();
                op_bitsRead += 1;
            }

            int myInt;
        };
    }
}
