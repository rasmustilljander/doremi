#include <Internal/LoggingModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <iostream>
#include <Windows.h>

namespace DoremiEngine
{
    namespace Logging
    {
        LoggingModuleImplementation::LoggingModuleImplementation() {}

        LoggingModuleImplementation::~LoggingModuleImplementation() {}

        void LoggingModuleImplementation::Startup() {}

        void LoggingModuleImplementation::Shutdown() {}
    }
}

DoremiEngine::Logging::LoggingModule* CreateLoggingModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Logging::LoggingModule* logger = new DoremiEngine::Logging::LoggingModuleImplementation();
    return logger;
}