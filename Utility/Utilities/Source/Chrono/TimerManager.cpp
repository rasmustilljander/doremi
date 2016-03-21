#include <Chrono/TimerManager.hpp>

namespace Doremi
{
    namespace Utilities
    {
        namespace Chrono
        {
            TimerManager::TimerManager() {}

            TimerManager::~TimerManager() {}

            void TimerManager::SaveTime(const std::string& p_name, double p_time)
            {
                if(m_stop)
                {
                    return;
                }
                auto& time = m_times.find(p_name);
                if(time != m_times.end())
                {
                    m_times.emplace(p_name, p_time);
                }
                else
                {
                    m_times[p_name] += p_time;
                }
            }
        }
    }
}