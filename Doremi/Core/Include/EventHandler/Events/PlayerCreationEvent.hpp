#pragma once
#include <EventHandler/Events/Event.hpp>
/**
Only an example of what an event might look like
Not needed for project. Could be removed when examples are no longer needed*/

namespace Doremi
{
    namespace Core
    {
        struct PlayerCreationEvent : public Event
        {
            PlayerCreationEvent(int p_playerEntityID) : Event(EventType::PlayerCreation), playerEntityID(p_playerEntityID) {}
            PlayerCreationEvent() : Event(EventType::PlayerCreation) {}

            /**
                Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override {}

            /**
                Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override {}

            int playerEntityID;
        };
    }
}
