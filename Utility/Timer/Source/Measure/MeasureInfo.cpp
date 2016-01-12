#include <Measure/MeasureInfo.hpp>

namespace Utility
{
    namespace Timer
    {

        MeasureInfo::MeasureInfo() : accumulatedTime(0.0) {}

        MeasureInfo::MeasureInfo(const std::string& p_name) : name(p_name), accumulatedTime(0.0f) {}

        MeasureInfo& MeasureInfo::Reset()
        {
            accumulatedTime.zero();
            return *this;
        }

        MeasureInfo& MeasureInfo::Start()
        {
            startSeconds = std::chrono::high_resolution_clock::now();
            return *this;
        }

        MeasureInfo& MeasureInfo::Stop()
        {
            stopSeconds = std::chrono::high_resolution_clock::now();

            accumulatedTime += (stopSeconds - startSeconds);
            startSeconds = stopSeconds;
            return *this;
        }

        double MeasureInfo::GetSeconds() const { return accumulatedTime.count(); }

        void MeasureInfo::SetChild(const MeasureInfo& p_measureInfo) { m_children.push_back(std::move(p_measureInfo)); }
    }
}