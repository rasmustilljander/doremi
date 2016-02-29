#include <Timing/TimerManager.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Logging/Include/LoggingModule.hpp>
#include <DoremiEngine/Logging/Include/SubmoduleManager.hpp>
#include <DoremiEngine/Logging/Include/Logger/Logger.hpp>

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

        void TimerManager::StopTimer(const std::string& p_name)
        {
            // See if timer exists
            auto& timer = m_timers.find(p_name);
            if(timer != m_timers.end())
            {
                //// Timer exists
                // Increase the counter for the timer
                timer->second.second += timer->second.first.Tick().GetElapsedTimeInSeconds();
            }
        }

        void TimerManager::StartTimer(const std::string& p_name)
        {
            // See if timer exists
            auto& timer = m_timers.find(p_name);
            if(timer != m_timers.end())
            {
                //// Timer exists, do nothing
            }
            else
            {
                //// Timer does not exist
                // Create new timer
                timer = m_timers.emplace(p_name, std::pair<Timer, double>(Timer(), 0)).first;
            }
            timer->second.first.Tick();
            ;
        }

        void TimerManager::ResetTimer(const std::string& p_name)
        {
            // See if timer exists
            auto& timer = m_timers.find(p_name);
            if(timer != m_timers.end())
            {
                //// Timer exists
                // Reset value
                timer->second.second = 0;
            }
        }

        void TimerManager::DumpData(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            using namespace Doremi::Utilities::Logging;
            auto& logger = p_sharedContext.GetLoggingModule().GetSubModuleManager().GetLogger();
            for(auto& i : m_timers)
            {
                logger.DebugLog(LogTag::GAME, LogLevel::MASS_DATA_PRINT, "%s:, %f", i.first.c_str(), i.second.second);
            }
        }
    }
}