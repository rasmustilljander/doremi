#pragma once
#include <Measurement/TimeMeasurement.hpp>
#include <Internal/Timer/TimerImpl.hpp>

#include <string>

namespace DoremiEngine
{
    namespace Timing
    {
        class TimeMeasurementImpl : public TimeMeasurement
        {
        public:
            /**
            Constructor
            */
            TimeMeasurementImpl();

            /**
            Constructor
            */
            TimeMeasurementImpl(const std::string& p_name);

            /**
            Resets the timer
            */
            TimeMeasurementImpl& Reset();

            /**
            Starts the timer
            */
            TimeMeasurementImpl& Start();

            /**
            Stops the timer
            */
            TimeMeasurementImpl& Stop();

            /**
            Gets the elapsed seconds since started
            */
            const double& GetElapsedSeconds() const override;

            const std::string& GetName() const override { return m_name; }
            const size_t& GetStartCount() const override { return m_startCount; }
            const size_t& GetStopCount() const override { return m_stopCount; }

        protected:
            // TOODRT As it were an struct earlier the names are incorrect.
            std::string m_name;
            double m_accumulatedTime;
            size_t m_startCount, m_stopCount;
            TimerImpl m_timer;
        };
    }
}
