#pragma once
namespace Doremi
{
    namespace Core
    {
        /** Every new type of event needs to be inserted in this enum*/
        enum class Events
        {
            Example, // Only an example
            PlayerCreation,
        };
        /**All events needs to inherit this struct*/
        struct Event
        {
            // Since all events inherits this struct we can store the event type here
            Events eventType;
        };
    }
}
