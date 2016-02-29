#pragma once
// Project specific
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
#include <Doremi/Core/Include/Helper/DoremiStates.hpp>
// Standard Libraries
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
        struct TriggerEvent;
        class InputHandler;
        class StateHandler : public Subscriber
        {
        public:
            /** Is a singleton. Use this method to get the EventManager*/
            static StateHandler* GetInstance();
            static void StartStateHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            static void StopStateHandler();
            ~StateHandler();
            DoremiGameStates GetState() const { return m_state; };
        private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;
            StateHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            static StateHandler* m_singleton;
            void OnEvent(Event* p_event) override;
            void CheckTrigger(TriggerEvent* p_event);
            DoremiGameStates m_state;
            int m_channel; // Used for playing the correct background sound for the state
        };
    }
}
