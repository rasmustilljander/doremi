#pragma once
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>

namespace Doremi
{
    namespace Core
    {
        class HealthChecker : public Subscriber
        {
        public:
            static HealthChecker* GetInstance();

            /**
                TODOCM doc
            */
            void Update();

            void OnEvent(Event* p_event) override;

        private:
            HealthChecker();
            ~HealthChecker();

            static HealthChecker* m_singleton;
        };
    }
}
