#pragma once
#include <EventHandler/Events/Event.hpp>
#include <ServerStateHandler.hpp>


namespace Doremi
{
    namespace Core
    {
        struct LoadNewWorldEvent : public Event
        {
            LoadNewWorldEvent() : Event(EventType::LoadNewWorld) {}

            /**
            Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override {}

            /**
            Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override {}

            GameMap map;
        };
    }
}
