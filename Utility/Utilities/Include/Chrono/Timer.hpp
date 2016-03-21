#pragma once

#include <chrono>
#include <String>

namespace Doremi
{
    namespace Utilities
    {
        namespace Chrono
        {
            class TimerManager;

            class Timer
            {
            public:
                Timer(TimerManager* p_timerManager, const std::string& p_name);
                Timer();
                virtual ~Timer();

                /**
                Resets the timer
                */
                void Reset();

                /**
                A tick the timer
                */
                Timer& Tick();

                /**
                Gets measured time between the two last calls to the function "tick", does not tick itself.
                */
                double GetElapsedTimeInSeconds() const;

            private:
                std::chrono::time_point<std::chrono::high_resolution_clock> m_current, m_previous;
                TimerManager* m_timerManager;
                std::string m_name;
            };
        }
    }
}