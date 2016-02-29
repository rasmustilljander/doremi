#pragma once
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>


namespace Doremi
{
    namespace Core
    {
        class EventHandlerClient : public EventHandler
        {
        public:
            static void StartupEventHandlerClient();

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
            void DeliverEvents();

            uint32_t GetNumberOfEvents();

        private:
            EventHandlerClient();
            ~EventHandlerClient();

            /**
                Mailbox for storing all events for later delivery
            */
            vector<Event*> m_eventBox;
        };
    }
}
