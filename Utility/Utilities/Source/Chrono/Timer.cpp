#include <Chrono/Timer.hpp>

namespace Doremi
{
    namespace Utilities
    {
        namespace Chrono
        {
            Timer::Timer() { Reset(); }

            Timer::~Timer() {}

            void Timer::Reset() { Tick(); }

            Timer& Timer::Tick()
            {
                using namespace std::chrono;
                m_previous = m_current;
                m_current = high_resolution_clock::now();
                return *this;
            }

            double Timer::GetElapsedTimeInSeconds() const
            {
                using namespace std::chrono;
                duration<double> duration = m_current - m_previous;
                return duration.count();
            }
        }
    }
}