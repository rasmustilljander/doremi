#pragma once

#include <chrono>

namespace Doremi
{
    namespace Utilities
    {
        namespace Chrono
        {

            class Timer
            {
            public:
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
            };
        }
    }
}