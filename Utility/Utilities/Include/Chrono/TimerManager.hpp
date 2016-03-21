#pragma once

#include <map>
#include <string>

namespace Doremi
{
    namespace Utilities
    {
        namespace Chrono
        {
            class TimerManager
            {
            public:
                TimerManager();
                virtual ~TimerManager();

                void SaveTime(const std::string& p_name, double p_time);

                void Stop() { m_stop = true; }

                const std::map<std::string, double>& GetAllData() const { return m_times; }

            private:
                std::map<std::string, double> m_times;
                bool m_stop = false;
            };
        }
    }
}