#include <Internal/TimingModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <iostream>
#include <Windows.h>

namespace DoremiEngine
{
    namespace Timing
    {
        TimingModuleImplementation::TimingModuleImplementation() {}

        TimingModuleImplementation::~TimingModuleImplementation() {}

        void TimingModuleImplementation::Startup() {}

        void TimingModuleImplementation::Shutdown() {}
    }
}

DoremiEngine::Timing::TimingModule* CreateTimingModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Timing::TimingModule* timer = new DoremiEngine::Timing::TimingModuleImplementation();
    return timer;
}