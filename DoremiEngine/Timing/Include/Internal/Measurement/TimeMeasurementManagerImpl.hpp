#pragma once
#include <string>
#include <unordered_map>
#include <DoremiEngine/Timing/Include/Measurement/TimeMeasurementManager.hpp>
#include <DoremiEngine/Timing/Include/Internal/Measurement/TimeMeasurementImpl.hpp>

namespace DoremiEngine
{
    namespace Timing
    {
        class TimeMeasurementManagerImpl : public TimeMeasurementManager
        {
        public:
            /**
                TODORT docs
            */
            TimeMeasurementManagerImpl::TimeMeasurementManagerImpl();

            /**
                TODORT docs
            */
            TimeMeasurementManagerImpl::~TimeMeasurementManagerImpl();


            /**
                Gets a timemeasurement with a given name
            */
            TimeMeasurement& GetTimeMeasurement(const std::string& p_name) override;

            /**
                TODORT docs
            */
            void DumpData(const std::string& p_origin) override;

        private:
            std::unordered_map<std::string, TimeMeasurementImpl> m_timeMeasurements;
        };
    }
}
