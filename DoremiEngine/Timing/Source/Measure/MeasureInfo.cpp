#include <Measure/MeasureInfo.hpp>

namespace DoremiEngine
{
    namespace Timing
    {

        MeasureInfo::MeasureInfo() : accumulatedTime(0.0), m_startCount(0), m_stopCount(0) {}

        MeasureInfo::MeasureInfo(const std::string& p_name) : name(p_name), accumulatedTime(0.0f), m_startCount(0), m_stopCount(0) {}

        MeasureInfo& MeasureInfo::Reset()
        {
            accumulatedTime.zero();
            return *this;
        }

        MeasureInfo& MeasureInfo::Start()
        {
            ++m_startCount;
            startSeconds = std::chrono::high_resolution_clock::now();
            return *this;
        }

        MeasureInfo& MeasureInfo::Stop()
        {
            ++m_stopCount;
            stopSeconds = std::chrono::high_resolution_clock::now();
            accumulatedTime += (stopSeconds - startSeconds);
            startSeconds = stopSeconds;
            return *this;
        }

        double MeasureInfo::GetSeconds() const { return accumulatedTime.count(); }
    }
}