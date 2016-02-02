#pragma once
#include <string>
#include <unordered_map>
#include <DoremiEngine\Timing\Include\Measure\MeasureInfo.hpp>

#define FILE_AND_FUNC std::string(__FILE__) + ":" + std::string(__func__)

#ifdef CUSTOM_TIMING_MEASUREMENT
// TODORT
// TODOLOG
//#define TIME_FUNCTION_START Utility::Timer::MeasureTimer::GetInstance().GetTimer(FILE_AND_FUNC).Start();
//#define TIME_FUNCTION_STOP Utility::Timer::MeasureTimer::GetInstance().GetTimer(FILE_AND_FUNC).Stop();
#define TIME_FUNCTION_START ;
#define TIME_FUNCTION_STOP ;
#else
#define TIME_FUNCTION_START ;
#define TIME_FUNCTION_STOP ;
#endif

namespace DoremiEngine
{
    namespace Timing
    {

        class MeasureTimer
        {
        public:
            /**
                Gets an instance of this object, if no instance exists, it creates one.
            */
            static MeasureTimer& GetInstance();

            /**
                Gets a timer with a given name
            */
            MeasureInfo& GetTimer(const std::string& p_name);

            /**
                TODORT docs
            */
            void DumpData(const std::string& p_origin);

        protected:
            MeasureTimer();
            MeasureTimer(MeasureTimer const&) = delete;
            void operator=(MeasureTimer const&) = delete;
            std::unordered_map<std::string, MeasureInfo> m_timers;
        };
    }
}
