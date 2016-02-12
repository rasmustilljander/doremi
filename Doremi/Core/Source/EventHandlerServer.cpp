#pragma once
// Project specific
#include <EventHandler/Events/Event.hpp>
#include <EventHandler/EventHandlerServer.hpp>
#include <EventHandler/Subscriber.hpp>

// Timing
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>

namespace Doremi
{
    namespace Core
    {
        EventHandlerServer::EventHandlerServer() {}

        EventHandlerServer::~EventHandlerServer() {}

        void EventHandlerServer::StartupEventHandlerServer()
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("EventHandlerServer StartupEventHandlerServer called multiple times.");
            }
            m_singleton = new EventHandlerServer();
        }

        void EventHandlerServer::BroadcastEvent(Event* p_event)
        {
            // Save the event for later delivery
            if(p_event->eventType != EventType::RemoveEntity)
            {
                m_basicEventBox.push_back(p_event);
            }
            else
            {
                m_removeEventBox.push_back(p_event);
            }
        }

        void EventHandlerServer::BroadcastEvent(std::list<Event*>& p_events)
        {
            // Save the event for later delivery
            for(std::list<Event*>::iterator iter = p_events.begin(); iter != p_events.end(); ++iter)
            {
                // Save the event for later delivery
                if((*iter)->eventType != EventType::RemoveEntity)
                {
                    m_basicEventBox.push_back(*iter);
                }
                else
                {
                    m_removeEventBox.push_back(*iter);
                }
            }
        }

        void EventHandlerServer::DeliverBasicEvents() // TODOJB fix broadcast event when no one is subscribed to them (currently crashes if you do)
        {
            TIME_FUNCTION_START

            // Iterate through all events
            size_t length = m_basicEventBox.size();
            for(std::vector<Doremi::Core::Event*>::iterator iter = m_basicEventBox.begin(); iter != m_basicEventBox.end(); ++iter)
            // for(size_t i = 0; i < length; i++) // need to check size if a new event is cast during delivery
            {
                // Iterate through all systems the event is to be broadcasted to
                unordered_map<EventType, vector<Subscriber*>>::iterator t_iter =
                    m_broadcastMap.find((*iter)->eventType); // Gets the vector of systems to call OnEvent on

                if(t_iter != m_broadcastMap.end())
                {
                    size_t nrOfSystems = t_iter->second.size();
                    for(size_t j = 0; j < nrOfSystems; j++)
                    {
                        t_iter->second[j]->OnEvent((*iter));
                    }
                    // Cleanup all data in event payload
                }

                delete(*iter);
            }

            m_basicEventBox.clear();
            TIME_FUNCTION_STOP
        }

        void EventHandlerServer::DeliverRemoveEvents() // TODOJB fix broadcast event when no one is subscribed to them (currently crashes if you do)
        {
            TIME_FUNCTION_START

            // Iterate through all events
            size_t length = m_removeEventBox.size();
            for(std::vector<Doremi::Core::Event*>::iterator iter = m_removeEventBox.begin(); iter != m_removeEventBox.end(); ++iter)
            // for(size_t i = 0; i < length; i++) // need to check size if a new event is cast during delivery
            {
                // Iterate through all systems the event is to be broadcasted to
                unordered_map<EventType, vector<Subscriber*>>::iterator t_iter =
                    m_broadcastMap.find((*iter)->eventType); // Gets the vector of systems to call OnEvent on

                if(t_iter != m_broadcastMap.end())
                {
                    size_t nrOfSystems = t_iter->second.size();
                    for(size_t j = 0; j < nrOfSystems; j++)
                    {
                        t_iter->second[j]->OnEvent((*iter));
                    }
                    // Cleanup all data in event payload
                }

                delete(*iter);
            }

            m_removeEventBox.clear();
            TIME_FUNCTION_STOP
        }
    }
}
