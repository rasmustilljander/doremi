#pragma once

namespace DoremiEngine
{
    namespace Timing
    {
        class Timer
        {
        public:
            /**
            Resets the timer
            */
            virtual void Reset() = 0;

            /**
            A tick the timer
            */
            virtual Timer& Tick() = 0;

            /**
            Gets time since last tick
            */
            virtual double GetElapsedTimeInSeconds() const = 0;
        };
    }
}
