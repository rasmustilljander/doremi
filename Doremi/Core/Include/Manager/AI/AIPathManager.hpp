#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>


namespace DoremiEngine
{
    namespace AI
    {
        class PotentialField;
    }
}

namespace Doremi
{
    namespace Core
    {
        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary*/
        class AIPathManager : public Manager, public Subscriber
        {
            public:
            AIPathManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~AIPathManager();
            void Update(double p_dt) override;
            // If the manager is a Subscriber, inherits the subscriber class, it will have to override OnEvent funciton
            void OnEvent(Event* p_event) override;

            private:
            DoremiEngine::AI::PotentialField* m_field;
            bool firstUpdate;
        };
    }
}
