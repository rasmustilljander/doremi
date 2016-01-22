#pragma once
/// Project specific
#include <Handler/StateHandler.hpp>
#include <EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>
// Events
#include <Doremi/Core/Include/EventHandler/Events/ChangeMenuState.hpp>
#include <Doremi/Core/Include/EventHandler/Events/TriggerEvent.hpp>

/// Standard Libraries
#include <unordered_map>
#include <vector>
#include <DirectXMath.h>


namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        StateHandler* StateHandler::m_singleton = nullptr;
        StateHandler* StateHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("StateHandler called GetInstance befor startup.");
            }
            return m_singleton;
        }
        void StateHandler::StartStateHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new StateHandler(p_sharedContext);
            }
            else
            {
                // log in personal logger, not critical
            }
        }
        void StateHandler::StopStateHandler()
        {
            // TODOKO implement
        }
        StateHandler::StateHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            EventHandler::GetInstance()->Subscribe(EventType::ChangeMenuState, this);
            EventHandler::GetInstance()->Subscribe(EventType::Trigger, this);
            m_state = DoremiStates::MAINMENU;
        }
        void StateHandler::OnEvent(Event* p_event)
        {
            std::map<uint32_t, Player*> t_players = PlayerHandler::GetInstance()->GetPlayerMap();
            if(p_event->eventType == EventType::ChangeMenuState)
            {
                ChangeMenuState* realEvent = static_cast<ChangeMenuState*>(p_event);
                m_state = realEvent->state;
            }
            else if(p_event->eventType == EventType::Trigger)
            {
                TriggerEventStruct* realEvent = static_cast<TriggerEventStruct*>(p_event);
                CheckTrigger(realEvent);
            }
        }
        void StateHandler::CheckTrigger(TriggerEventStruct* p_event)
        {
            // Does this work?
            TriggerType type = p_event->triggerType;
            if(((int)type & (int)TriggerType::GoalTrigger) == (int)TriggerType::GoalTrigger)
            {
                ChangeMenuState* menuStateEvent =
                    new ChangeMenuState(); // Send event so we easily can call the event changing algorithm to see if the new state is valid
                menuStateEvent->state = DoremiStates::MAINMENU;
                EventHandler::GetInstance()->BroadcastEvent(menuStateEvent); // TODOKO review if this is a good idea
                // m_state = DoremiStates::MAINMENU;
            }
        }
    }
}
