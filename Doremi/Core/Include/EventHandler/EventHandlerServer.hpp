#pragma once
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>


namespace Doremi
{
    namespace Core
    {
        class EventHandlerServer : public EventHandler
        {
        public:
            static void StartupEventHandlerServer();

            /**
            Puts event in mailbox to be handled by the event handler
            */
            void BroadcastEvent(Event* p_event) override;

            /**
            Puts events in mailbox to be handled by the event handler
            */
            void BroadcastEvent(std::list<Event*>& p_events) override;

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
            EventHandlerServer();
            ~EventHandlerServer();

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
