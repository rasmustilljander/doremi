#pragma once
#include <EventHandler/Events/Event.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TriggerComponent.hpp>
/**
Only an example of what an event might look like
Not needed for project. Could be removed when examples are no longer needed*/

namespace Doremi
{
    namespace Core
    {
        struct TriggerEventStruct : public Event
        {
            TriggerEventStruct() : Event(EventType::Trigger) { entityID = 0, triggerType = TriggerType::NoTrigger; }

            /**
                Write object to stream
            */
            void Write(Streamer* p_streamer, uint32_t& op_bitsWritten) override {}

            /**
                Read object from stream
            */
            void Read(Streamer* p_streamer, uint32_t& op_bitsRead) override {}

            int entityID;
            TriggerType triggerType;
        };
    }
}
