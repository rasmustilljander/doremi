#pragma once
#include <time.h>
#include <string>
#include <unordered_map>

namespace Utility
{
    namespace Timer
    {
        struct MeasureInfo
        {
            MeasureInfo() : startSeconds(0.0f), stopSeconds(0.0f), accumulatedTime(0.0f) {}

            MeasureInfo(const std::string& p_name) : name(p_name), startSeconds(0.0f), stopSeconds(0.0f), accumulatedTime(0.0f) {}

            void Reset() { startSeconds = stopSeconds = accumulatedTime = 0.0f; }
            std::string name;
            clock_t startSeconds;
            clock_t stopSeconds;
            float accumulatedTime;
        };

        class MeasureTimer
        {
            public:
            /**
            TODORT docs
            */
            static MeasureTimer& GetInstance()
            {
                static MeasureTimer instance;
                return instance;
            }

            /**
            TODORT docs
            */
            MeasureTimer(MeasureTimer const&) = delete;

            /**
            TODORT docs
            */
            void operator=(MeasureTimer const&) = delete;

            /**
            TODORT docs
            */
            MeasureTimer& Reset(const std::string& p_name);

            /**
            TODORT docs
            */
            MeasureTimer& Start(const std::string& p_name);
            MeasureTimer& Stop(const std::string& p_name);

            /**
            TODORT docs
            */
            float GetSeconds(const std::string& p_name);

            /**
            TODORT docs
            */
            void DumpData();

            private:
            MeasureTimer();
            std::unordered_map<std::string, MeasureInfo> m_timers;
        };
    }
}
