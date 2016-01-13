#include <Utility/Timer/Include/Measure/MeasureTimer.hpp>
#include <exception>
#include <Utility/DebugLog/Include/ConsoleManager.hpp>

namespace Utility
{
    namespace Timer
    {
        MeasureTimer& MeasureTimer::GetInstance()
        {
            // TODORT Currently there's no way of releasing this singleton.
            static MeasureTimer instance;
            return instance;
        }

        MeasureTimer::MeasureTimer() {}

        MeasureInfo& MeasureTimer::GetTimer(const std::string& p_name)
        {
            if(m_timers.count(p_name) == 0)
            {
                m_timers[p_name] = MeasureInfo(p_name);
            }
            return m_timers[p_name];
        }

        void MeasureTimer::DumpData(const std::string& p_origin)
        {
            using namespace Utility::DebugLog;
            VirtualConsole& console = ConsoleManager::GetInstance().CreateNewConsole(p_origin, false);

            for(auto current : m_timers)
            {
                auto a = current.second;
                console.LogText(LogTag::GENERAL, LogLevel::INFO, "Totaltime: %f, StartCount: %u, StopCount: %u, Location: %s: ", a.GetSeconds(),
                                a.GetStartCount(), a.GetStopCount(), a.GetName().c_str());
            }
        }
    }
}