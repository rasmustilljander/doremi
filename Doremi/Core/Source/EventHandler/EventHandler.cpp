#pragma once
// Project specific
#include <EventHandler/Events/Event.hpp>
#include <EventHandler/EventHandler.hpp>
#include <EventHandler/Subscriber.hpp>

namespace Doremi
{
    namespace Core
    {
        EventHandler* EventHandler::m_singleton = nullptr;

        EventHandler* EventHandler::GetInstance()
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("EventHandler not initialized, GetInstance called.");
            }
            return m_singleton;
        }

        EventHandler::EventHandler() {}


        EventHandler::~EventHandler() {}


        void EventHandler::Subscribe(EventType p_eventType, Subscriber* p_subscriber) { m_broadcastMap[p_eventType].push_back(p_subscriber); }
    }
}
