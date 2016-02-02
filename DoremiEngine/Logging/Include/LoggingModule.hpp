#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <vector>
#if defined(_WINDLL)
#define LOGGING_DLL_EXPORT __declspec(dllexport)
#else
#define LOGGING_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace Logging
    {
        /**
        TODO docs
        */
        class LoggingModule : public DoremiEngine::Core::EngineModule
        {
        public:
        };
    }
}

extern "C" {
typedef DoremiEngine::Logging::LoggingModule* (*CREATE_LOGGING_MODULE)(const DoremiEngine::Core::SharedContext&);
LOGGING_DLL_EXPORT DoremiEngine::Logging::LoggingModule* CreateLoggingModule(const DoremiEngine::Core::SharedContext& p_context);
}