#include <Internal/Measurement/TimeMeasurementManagerImpl.hpp>
#include <Internal/Measurement/TimeMeasurementImpl.hpp>

namespace DoremiEngine
{
    namespace Timing
    {
        TimeMeasurementManagerImpl::TimeMeasurementManagerImpl() {}

        TimeMeasurementManagerImpl::~TimeMeasurementManagerImpl() {}

        TimeMeasurement& TimeMeasurementManagerImpl::GetTimeMeasurement(const std::string& p_name)
        {
            if(m_timeMeasurements.count(p_name) == 0)
            {
                m_timeMeasurements[p_name] = TimeMeasurementImpl(p_name);
            }
            return m_timeMeasurements[p_name];
        }

        void TimeMeasurementManagerImpl::DumpData(const std::string& p_origin)
        {
            // TODOLOG
            // TODORT
            // using namespace Utility::DebugLog;
            // VirtualConsole& console = ConsoleManager::GetInstance().CreateNewConsole(p_origin, false);

            for(auto& current : m_timeMeasurements)
            {
                // TODOLOG
                // TODORT
                auto a = current.second;
                //    console.LogText(LogTag::GENERAL, LogLevel::INFO, "Totaltime: %f, StartCount: %u, StopCount: %u, Location: %s: ", a.GetSeconds(),
                //                   a.GetStartCount(), a.GetStopCount(), a.GetName().c_str());
            }
        }
    }
}