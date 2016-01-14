#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>

namespace Doremi
{
    namespace Core
    {
        /**
        Updates the force on the entity by the accelleration and direction
        */
        class MovementManager : public Manager, public Subscriber
        {
        public:
            MovementManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~MovementManager();
            void Update(double p_dt) override;
            // If the manager is a Subscriber, inherits the subscriber class, it will have to override OnEvent funciton
            void OnEvent(Event* p_event) override;

        private:
        };
    }
}
