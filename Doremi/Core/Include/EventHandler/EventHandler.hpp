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
            virtual void BroadcastEvent(Event* p_event) = 0;

            /**
                Puts events in mailbox to be handled by the event handler
            */
            virtual void BroadcastEvent(std::list<Event*>& p_events) = 0;

            /**
                Subscribes to a certain event type
            */
            void Subscribe(EventType p_eventType, Subscriber* p_subscriber);

        protected:
            // Private constructors because Singleton
            EventHandler();
            ~EventHandler();

            static EventHandler* m_singleton;
            /**
                A map between the event type and the classes subscribing to the event
            */
            unordered_map<EventType, vector<Subscriber*>> m_broadcastMap;
        };
    }
}
