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

        void MeasureTimer::AddChildToParent(const std::string& p_parent, const std::string& p_child)
        {
            GetTimer(p_parent).SetChild(GetTimer(p_child));
        }

        void MeasureTimer::DumpData(const std::string& p_origin)
        {

            for(auto current : m_timers)
            {
                DumpChildrenData(p_origin, current.second, 0);
            }

            //      myConsole.LogText(LogTag::NOTAG, LogLevel::INFO, );
        }

        void MeasureTimer::DumpChildrenData(const std::string& p_origin, const MeasureInfo& p_parent, const size_t& p_index)
        {
            using namespace Utility::DebugLog;
            static VirtualConsole& console = ConsoleManager::GetInstance().CreateNewConsole(p_origin, false);

            const float parentTime = p_parent.GetSeconds();
            console.LogText(LogTag::GENERAL, LogLevel::INFO, "%s: %f", p_parent.GetName().c_str(), parentTime);
            const std::vector<MeasureInfo> m_children = p_parent.GetAllChildren();
            for(auto child : m_children)
            {
                DumpChildrenData(p_origin, child, p_index + 1);
            }
        }
    }
}