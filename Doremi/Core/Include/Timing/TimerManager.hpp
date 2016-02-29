#pragma once
#include <unordered_map>
#include <Utility/Utilities/Include/Chrono/Timer.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

#define FILE_AND_FUNC std::string(__FILE__) + ":" + std::string(__func__)

#ifdef CUSTOM_TIMING_MEASUREMENT
#define TIME_FUNCTION_START Doremi::Core::TimerManager::GetInstance().StartTimer(FILE_AND_FUNC);
#define TIME_FUNCTION_STOP Doremi::Core::TimerManager::GetInstance().StopTimer(FILE_AND_FUNC);
#else
#define TIME_FUNCTION_START ;
#define TIME_FUNCTION_STOP ;
#endif

namespace Doremi
{
    namespace Core
    {
        class TimerManager
        {
        public:
            TimerManager(const TimerManager& p_timerManager) = delete;
            void operator=(const TimerManager&) = delete;

            /**
            TODO docs
            */
            static TimerManager& GetInstance();

            /**
            TODO docs
            */
            void StartTimer(const std::string& p_name);

            /**
            TODO docs
            */
            void StopTimer(const std::string& p_name);

            /**
            TODO docs
            */
            void ResetTimer(const std::string& p_name);

            /**
            TODO docs
            */
            void DumpData(const DoremiEngine::Core::SharedContext& p_sharedContext);

        private:
            TimerManager();
            virtual ~TimerManager();

            std::unordered_map<std::string, std::pair<Utilities::Chrono::Timer, double>> m_timers;
        };
    }
}