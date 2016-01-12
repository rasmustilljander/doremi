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

            /**
            Set a child timer.
            */
            void SetChild(const MeasureInfo& p_measureInfo);

            const std::vector<MeasureInfo>& GetAllChildren() const { return m_children; }

            const std::string& GetName() const { return name; }

            protected:
            // TOODRT As it were an struct earlier the names are incorrect.
            std::string name;
            std::chrono::time_point<std::chrono::high_resolution_clock> startSeconds, stopSeconds;
            std::chrono::duration<double> accumulatedTime;
            //  double accumulatedTime;
            std::vector<MeasureInfo> m_children;
        };
    }
}
