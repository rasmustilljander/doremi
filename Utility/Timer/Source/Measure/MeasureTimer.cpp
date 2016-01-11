#include <Utility/Timer/Include/Measure/MeasureTimer.hpp>

namespace Utility
{
    namespace Timer
    {
        MeasureTimer::MeasureTimer() {}

        MeasureTimer& MeasureTimer::Reset(const std::string& p_name)
        {
            if(m_timers.count(p_name) == 0)
            {
                m_timers[p_name] = MeasureInfo(p_name);
            }
            else
            {
                m_timers[p_name].Reset();
            }
            return *this;
        }

        MeasureTimer& MeasureTimer::Start(const std::string& p_name)
        {
            m_timers[p_name].startSeconds = clock();
            return *this;
        }

        MeasureTimer& MeasureTimer::Stop(const std::string& p_name)
        {
            m_timers[p_name].stopSeconds = clock();

            m_timers[p_name].accumulatedTime += (m_timers[p_name].stopSeconds - m_timers[p_name].startSeconds) / (float)CLOCKS_PER_SEC;
            m_timers[p_name].startSeconds = m_timers[p_name].stopSeconds;
            return *this;
        }

        float MeasureTimer::GetSeconds(const std::string& p_name) { return m_timers[p_name].accumulatedTime; }
        void MeasureTimer::DumpData() {}
    }
}