#pragma once
// Project specific
//#include <EventHandler/Events/Event.hpp>

// Standard Libraries
#include <map>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        struct Event;
        enum class Events;
        class Subscriber;
        class EventHandler
        {
            public:
            // Private constructors because Singleton
            EventHandler();
            ~EventHandler();

            /** Is a singleton. Use this method to get the EventManager*/
            // static EventHandler* GetInstance();
            /** Puts event in mailbox to be handled by the event handler*/
            void BroadcastEvent(Event* p_event);
            /** Subscribes to a certain event type*/
            void Subscribe(Events p_eventType, Subscriber* p_subscriber);
            /** Begin processing events by calling OnEvent in all subscribers*/
            void DeliverEvents();

            private:
            // static EventHandler* m_singleton;
            // A map between the event type and the classes subscribing to the event
            unordered_map<Events, vector<Subscriber*>> m_broadcastMap;
            // Big ass mailbox for storing the events for later delivery
            vector<Event*> m_mailBox;
        };
    }
}
