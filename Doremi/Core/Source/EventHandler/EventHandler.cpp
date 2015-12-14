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
            if(m_singleton == nullptr)
            {
                m_singleton = new EventHandler();
            }
            return m_singleton;
        }

        EventHandler::EventHandler() {}


        EventHandler::~EventHandler() {}

        void EventHandler::BroadcastEvent(Event* p_event)
        {
            // Save the event for later delivery
            m_mailBox.push_back(p_event);
        }

        void EventHandler::Subscribe(Events p_eventType, Subscriber* p_subscriber) { m_broadcastMap[p_eventType].push_back(p_subscriber); }

        void EventHandler::DeliverEvents() // TODOJB fix broadcast event when no one is subscribed to them (currently crashes if you do)
        {
            // Iterate through all events
            size_t length = m_mailBox.size();
            for(size_t i = 0; i < length; i++)
            {
                // Iterate through all systems the event is to be broadcasted to
                unordered_map<Events, vector<Subscriber*>>::iterator t_iter =
                    m_broadcastMap.find(m_mailBox[i]->eventType); // Gets the vector of systems to call OnEvent on
                size_t nrOfSystems = t_iter->second.size();
                for(size_t j = 0; j < nrOfSystems; j++)
                {
                    t_iter->second[j]->OnEvent(m_mailBox[i]);
                }
                // Cleanup all data in event payload
                delete m_mailBox[i];
            }

            m_mailBox.clear();
        }
    }
}
