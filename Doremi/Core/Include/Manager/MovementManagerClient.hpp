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
        class MovementManagerClient : public Manager, public Subscriber
        {
        public:
            MovementManagerClient(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~MovementManagerClient();
            void Update(double p_dt) override;
            // If the manager is a Subscriber, inherits the subscriber class, it will have to override OnEvent funciton
            void OnEvent(Event* p_event) override;

        private:
        };
    }
}
