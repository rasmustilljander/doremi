#pragma once

#include <Timer/Timer.hpp>
#include <chrono>

namespace DoremiEngine
{
    namespace Timing
    {
        class TimerImpl : public Timer
        {
        public:
            TimerImpl();
            virtual ~TimerImpl();

            /**
            Resets the timer
            */
            void Reset() override;

            /**
            A tick the timer
            */
            Timer& Tick() override;

            /**
            Gets measured time between the two last calls to the function "tick", does not tick itself.
            */
            double GetElapsedTimeInSeconds() const override;

        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> m_current, m_previous;
        };
    }
}