#pragma once
#include <unordered_map>
#include <TimingModule.hpp>

namespace DoremiEngine
{
    namespace Timing
    {
        class TimingModuleImplementation : public TimingModule
        {

        public:
            /**
                TODO docs
            */
            TimingModuleImplementation();

            /**
                TODO docs
            */
            virtual ~TimingModuleImplementation();

            /**
                  TODO docs
            */
            void Startup() override;

            /**
              TODO docs
            */
            void Shutdown() override;
        };
    }
}