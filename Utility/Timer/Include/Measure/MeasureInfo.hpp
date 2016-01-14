#pragma once

#include <string>
#include <vector>
#include <chrono>

namespace Utility
{
    namespace Timer
    {
        class MeasureInfo
        {
        public:
            /**
                Constructor
            */
            MeasureInfo();

            /**
    Constructor
*/
            MeasureInfo(const std::string& p_name);

            /**
    Resets the timer
*/
            MeasureInfo& Reset();

            /**
            Starts the timer
            */
            MeasureInfo& Start();

            /**
            Stops the timer
            */
            MeasureInfo& Stop();

            /**
            Gets the elapsed seconds since started
            */
            double GetSeconds() const;

            const std::string& GetName() const { return name; }
            const size_t& GetStartCount() const { return m_startCount; }
            const size_t& GetStopCount() const { return m_stopCount; }

        protected:
            // TOODRT As it were an struct earlier the names are incorrect.
            std::string name;
            std::chrono::time_point<std::chrono::high_resolution_clock> startSeconds, stopSeconds;
            std::chrono::duration<double> accumulatedTime;
            //  double accumulatedTime;
            size_t m_startCount, m_stopCount;
        };
    }
}
