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
        enum class EventType;
        class Subscriber;
        class EventHandler
        {
        public:
            /**
                Is a singleton. Use this method to get the EventManager
            */
            static EventHandler* GetInstance();

            /**
                Puts event in mailbox to be handled by the event handler
            */
            void BroadcastEvent(Event* p_event);

            /**
                Subscribes to a certain event type
            */
            void Subscribe(EventType p_eventType, Subscriber* p_subscriber);

            /**
                Begin processing basic events by calling OnEvent in all subscribers
            */
            void DeliverBasicEvents();

            /**
                Begin processing remove events by calling OnEvent in all subscribers
            */
            void DeliverRemoveEvents();

        private:
            // Private constructors because Singleton
            EventHandler();
            ~EventHandler();

            static EventHandler* m_singleton;
            /**
                A map between the event type and the classes subscribing to the event
            */
            unordered_map<EventType, vector<Subscriber*>> m_broadcastMap;

            /**
                Mailbox for storing the regular events(not add and remove) for later delivery
            */
            vector<Event*> m_basicEventBox;

            /**
                Mailbox for storing remove events for later delivery
            */
            vector<Event*> m_removeEventBox;
        };
    }
}
