#pragma once
#include <string>
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>

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
        class TimeMeasurement;

        class TimeMeasurementManager
        {
        public:
            /**
                Gets a timemeasurement with a given name
            */
            virtual TimeMeasurement& GetTimeMeasurement(const std::string& p_name) = 0;

            /**
                TODORT docs
            */
            virtual void DumpData(const std::string& p_origin) = 0;
        };
    }
}
