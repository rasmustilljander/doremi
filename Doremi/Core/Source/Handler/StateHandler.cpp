#pragma once
/// Project specific
#include <Handler/StateHandler.hpp>
#include <EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/PlayerHandler.hpp>
// Events
#include <Doremi/Core/Include/EventHandler/Events/ChangeMenuState.hpp>
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
                // TODOKO send excpetion
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
                // TODOKO send excpetion, already started
            }
        }
        void StateHandler::StopStateHandler()
        {
            // TODOKO implement
        }
        StateHandler::StateHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            EventHandler::GetInstance()->Subscribe(EventType::ChangeMenuState, this);
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
        }
    }
}
