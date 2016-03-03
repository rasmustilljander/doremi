#pragma once
#include <Doremi/Core/Include/Timing/TimerManager.hpp>
#include <string>

namespace Doremi
{
    namespace Core
    {
        struct NamedTimerRAII
        {
            NamedTimerRAII(const std::string& p_name) : name(p_name) { TimerManager::GetInstance().StartTimer(name); }

            ~NamedTimerRAII() { TimerManager::GetInstance().StopTimer(name); }
            std::string name;
        };
    }
}
#ifdef USE_CUSTOM_TIMER
#define NAMED_TIMER(X) Doremi::Core::NamedTimerRAII timer = Doremi::Core::NamedTimerRAII(X);
#else
#define NAMED_TIMER(X) ;
#endif