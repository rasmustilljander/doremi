#pragma once
#include <Doremi/Core/Include/Timing/TimerManager.hpp>
#include <string>

namespace Doremi
{
    namespace Core
    {
        struct FunctionTimerRAII
        {
            FunctionTimerRAII(const std::string& p_file, const std::string& p_function, const uint32_t p_line)
                : file(p_file), function(p_function), line(p_line), timer(TimerManager::GetInstance().StartTimer(file, function, line))
            {
            }

            ~FunctionTimerRAII() { TimerManager::GetInstance().StopTimer(timer); }
            std::string file;
            std::string function;
            uint32_t line;
            const TimerData* const timer;
        };
    }
}

#ifdef CUSTOM_TIMING_MEASUREMENT
#define FUNCTION_TIMER Doremi::Core::FunctionTimerRAII timer = Doremi::Core::FunctionTimerRAII(__FILE__, __func__, __LINE__);
#else
#define FUNCTION_TIMER ;
#endif
