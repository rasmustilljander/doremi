#include <Internal/Measurement/TimeMeasurementImpl.hpp>

namespace DoremiEngine
{
    namespace Timing
    {
        TimeMeasurementImpl::TimeMeasurementImpl() : m_accumulatedTime(0.0), m_startCount(0), m_stopCount(0) {}

        TimeMeasurementImpl::TimeMeasurementImpl(const std::string& p_name) : m_name(p_name), m_accumulatedTime(0.0), m_startCount(0), m_stopCount(0)
        {
        }

        TimeMeasurementImpl& TimeMeasurementImpl::Reset()
        {
            m_timer.Reset();
            return *this;
        }

        TimeMeasurementImpl& TimeMeasurementImpl::Start()
        {
            ++m_startCount;
            m_timer.Tick();
            return *this;
        }

        TimeMeasurementImpl& TimeMeasurementImpl::Stop()
        {
            ++m_stopCount;
            m_accumulatedTime += m_timer.Tick().GetElapsedTimeInSeconds();
            return *this;
        }
        const double& TimeMeasurementImpl::GetElapsedSeconds() const { return m_accumulatedTime; }
    }
}