#pragma once
#include <string>
#include <unordered_map>
#include <Utility/Timer/Include/Measure/MeasureInfo.hpp>

namespace Utility
{
    namespace Timer
    {

        class MeasureTimer
        {
            public:
            /**
                Gets an instance of this object, if no instance exists, it creates one.
            */
            static MeasureTimer& GetInstance();

            /**
                Gets a timer with a given name
            */
            MeasureInfo& GetTimer(const std::string& p_name);

            /**
            */
            void MeasureTimer::AddChildToParent(const std::string& p_parent, const std::string& p_child);

            /**
                TODORT docs
            */
            void DumpData(const std::string& p_origin);

            protected:
            MeasureTimer();
            MeasureTimer(MeasureTimer const&) = delete;
            void operator=(MeasureTimer const&) = delete;

            void DumpChildrenData(const std::string& p_origin, const MeasureInfo& p_parent, const size_t& p_index);


            std::unordered_map<std::string, MeasureInfo> m_timers;
        };
    }
}
