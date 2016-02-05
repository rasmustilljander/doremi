#pragma once
#include <string>
namespace DoremiEngine
{
    namespace Timing
    {
        class TimeMeasurement
        {
        public:
            /**
            Resets the timer
            */
            virtual TimeMeasurement& Reset() = 0;

            /**
            Starts the timer
            */
            virtual TimeMeasurement& Start() = 0;

            /**
            Stops the timer
            */
            virtual TimeMeasurement& Stop() = 0;

            /**
            Gets the elapsed seconds since started
            */
            virtual const double& GetElapsedSeconds() const = 0;

            virtual const std::string& GetName() const = 0;
            virtual const size_t& GetStartCount() const = 0;
            virtual const size_t& GetStopCount() const = 0;
        };
    }
}
