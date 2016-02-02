#include <Internal/Timer/TimerImpl.hpp>

namespace DoremiEngine
{
    namespace Timing
    {
        TimerImpl::TimerImpl() { Reset(); }

        TimerImpl::~TimerImpl() {}

        void TimerImpl::Reset() { Tick(); }

        Timer& TimerImpl::Tick()
        {
            using namespace std::chrono;
            m_previous = m_current;
            m_current = high_resolution_clock::now();
            return *this;
        }

        double TimerImpl::GetElapsedTimeInSeconds() const
        {
            using namespace std::chrono;
            duration<double> duration = m_current - m_previous;
            return duration.count();
        }
    }
}