#include <Chrono/Timer.hpp>
#include <Chrono/TimerManager.hpp>

namespace Doremi
{
    namespace Utilities
    {
        namespace Chrono
        {
            Timer::Timer() : m_timerManager(nullptr) { Reset(); }
            Timer::Timer(TimerManager* p_timerManager, const std::string& p_name) : m_timerManager(p_timerManager), m_name(p_name) { Reset(); }

            Timer::~Timer()
            {
                if(m_timerManager != nullptr)
                {
                    Tick();
                    m_timerManager->SaveTime(m_name, GetElapsedTimeInSeconds());
                    m_timerManager = nullptr;
                }
            }

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