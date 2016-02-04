#pragma once

namespace Doremi
{
    namespace Core
    {
        class HealthChecker
        {
        public:
            static HealthChecker* GetInstance();

            /**
                TODOCM doc
            */
            void Update();

        private:
            HealthChecker();
            ~HealthChecker();

            static HealthChecker* m_singleton;
        };
    }
}
