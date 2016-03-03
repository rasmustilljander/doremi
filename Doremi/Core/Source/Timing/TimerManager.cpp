#include <Timing/TimerManager.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Logging/Include/LoggingModule.hpp>
#include <DoremiEngine/Logging/Include/SubmoduleManager.hpp>
#include <DoremiEngine/Logging/Include/Logger/Logger.hpp>

#include <algorithm>
#include <vector>

namespace Doremi
{
    namespace Core
    {
        using namespace Doremi::Utilities::Chrono;

        TimerManager::TimerManager() {}

        TimerManager::~TimerManager() {}

        TimerManager& TimerManager::GetInstance()
        {
            static TimerManager timerManager;
            return timerManager;
        }

        const TimerData* const TimerManager::StartTimer(const std::string& p_file, const std::string& p_function, const uint32_t& p_lineNumber)
        {
            // See if timer exists
            auto& timer = m_timers.find(TimerKey(p_file, p_function, p_lineNumber));
            if(timer != m_timers.end())
            {
                //// Timer exists
                timer->second.timer.Tick();
                return &timer->second;
            }
            else
            {
                //// Timer does not exist
                // Create new timer
                auto& tTimer = m_timers.emplace(TimerKey(p_file, p_function, p_lineNumber), TimerData(p_lineNumber)).first->second;
                tTimer.timer.Tick();
                return &tTimer;
            }
        }

        void TimerManager::StopTimer(const TimerData* const p_timer)
        {
            TimerData* timer = const_cast<TimerData*>(p_timer);
            timer->data += timer->timer.Tick().GetElapsedTimeInSeconds();
        }

        void TimerManager::StartTimer(const std::string& p_name)
        {
            const auto& timer = m_namedTimers.find(p_name);
            if(timer != m_namedTimers.end())
            {
                //// Already exists
                timer->second.timer.Tick();
            }
            else
            {
                //// Does not already exist
                const auto& tTimer = m_namedTimers.emplace(p_name, TimerData(0));
                tTimer.first->second.timer.Tick();
            }
        }

        void TimerManager::StopTimer(const std::string& p_name)
        {
            const auto& timer = m_namedTimers.find(p_name);
            if(timer != m_namedTimers.end())
            {
                //// Already exists
                timer->second.data += timer->second.timer.Tick().GetElapsedTimeInSeconds();
            }
            else
            {
                const std::string message("Timer with name '" + p_name + "' does not exist.");
                throw std::exception(message.c_str());
            }
        }

        bool sortTimerData(const std::pair<TimerKey, TimerData>& lhs, const std::pair<TimerKey, TimerData>& rhs)
        {
            if(lhs.first.file < rhs.first.file)
            {
                return true;
            }
            else if(rhs.first.file < lhs.first.file)
            {
                return false;
            }
            else
            {
                if(rhs.first.file < lhs.first.file)
                {
                    return true;
                }
                else if(lhs.first.file < rhs.first.file)
                {
                    return false;
                }
                else
                {
                    if(rhs.second.startLine < lhs.second.startLine)
                    {
                        return true;
                    }
                    else if(lhs.second.startLine < rhs.second.startLine)
                    {
                        return false;
                    }
                }
            }
        }

        void TimerManager::DumpData(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            printf("Dumping data.\n");
            std::vector<std::pair<TimerKey, TimerData>> vectorEntries;
            vectorEntries.reserve(m_timers.size());
            for(const auto& entry : m_timers)
            {
                vectorEntries.push_back(std::pair<TimerKey, TimerData>(entry.first, entry.second));
            }

            std::sort(vectorEntries.begin(), vectorEntries.end(), sortTimerData);

            using namespace Doremi::Utilities::Logging;
            auto& logger = p_sharedContext.GetLoggingModule().GetSubModuleManager().GetLogger();
            for(const auto& i : vectorEntries)
            {
                logger.LogText(LogTag::TIMER, LogLevel::MASS_DATA_PRINT, "%s:%s:%d, %f", i.first.file.c_str(), i.first.function.c_str(),
                               i.second.startLine, i.second.data);
                printf("%s:%s:%d, %f\n", i.first.file.c_str(), i.first.function.c_str(), i.second.startLine, i.second.data);
            }

            for(const auto& i : m_namedTimers)
            {
                logger.LogText(LogTag::TIMER, LogLevel::MASS_DATA_PRINT, "%s, %f", i.first.c_str(), i.second.data);
                printf("%s, %f\n", i.first.c_str(), i.second.data);
            }
        }
    }
}