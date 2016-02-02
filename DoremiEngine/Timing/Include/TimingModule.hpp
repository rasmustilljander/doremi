#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <vector>
#if defined(_WINDLL)
#define TIMING_DLL_EXPORT __declspec(dllexport)
#else
#define TIMING_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace Timing
    {
        /**
        TODO docs
        */
        class TimingModule : public DoremiEngine::Core::EngineModule
        {
        public:
        };
    }
}

extern "C" {
typedef DoremiEngine::Timing::TimingModule* (*CREATE_TIMING_MODULE)(const DoremiEngine::Core::SharedContext&);
TIMING_DLL_EXPORT DoremiEngine::Timing::TimingModule* CreateTimingModule(const DoremiEngine::Core::SharedContext& p_context);
}