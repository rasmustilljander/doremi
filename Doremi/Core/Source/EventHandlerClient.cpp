#pragma once
// Project specific
#include <EventHandler/Events/Event.hpp>
#include <EventHandler/EventHandlerClient.hpp>
#include <EventHandler/Subscriber.hpp>

// Timing
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>

namespace Doremi
{
    namespace Core
    {
        EventHandlerClient::EventHandlerClient() {}

        EventHandlerClient::~EventHandlerClient() {}

        void EventHandlerClient::StartupEventHandlerClient()
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("EventHandlerClient StartupEventHandlerClient called multiple times.");
            }
            m_singleton = new EventHandlerClient();
        }

        void EventHandlerClient::BroadcastEvent(Event* p_event)
        {
            // Save the event for later delivery
            m_eventBox.push_back(p_event);
        }

        void EventHandlerClient::BroadcastEvent(std::list<Event*>& p_events)
        {
            // Save the event for later delivery
            for(std::list<Event*>::iterator iter = p_events.begin(); iter != p_events.end(); ++iter)
            {
                m_eventBox.push_back(*iter);
            }
        }

        void EventHandlerClient::DeliverEvents() // TODOJB fix broadcast event when no one is subscribed to them (currently crashes if you do)
        {
            TIME_FUNCTION_START

            // Iterate through all events
            size_t length = m_eventBox.size();
            for(size_t i = 0; i < length; i++) // need to check size if a new event is cast during delivery
            {
                // Iterate through all systems the event is to be broadcasted to
                unordered_map<EventType, vector<Subscriber*>>::iterator t_iter =
                    m_broadcastMap.find(m_eventBox[i]->eventType); // Gets the vector of systems to call OnEvent on

                if(t_iter != m_broadcastMap.end())
                {
                    size_t nrOfSystems = t_iter->second.size();
                    for(size_t j = 0; j < nrOfSystems; j++)
                    {
                        t_iter->second[j]->OnEvent(m_eventBox[i]);
                    }
                    // Cleanup all data in event payload
                }

                delete m_eventBox[i];
            }

            m_eventBox.clear();
            TIME_FUNCTION_STOP
        }
    }
}
