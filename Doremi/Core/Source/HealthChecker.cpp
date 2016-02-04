#include <Doremi/Core/Include/HealthChecker.hpp>

namespace Doremi
{
    namespace Core
    {
        HealthChecker* HealthChecker::m_singleton = nullptr;

        HealthChecker* HealthChecker::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new HealthChecker();
            }
            return m_singleton;
        }

        HealthChecker::HealthChecker() {}

        HealthChecker::~HealthChecker() {}

        void HealthChecker::Update()
        {
            // TODOXX doesn't exist, just template
        }
    }
}