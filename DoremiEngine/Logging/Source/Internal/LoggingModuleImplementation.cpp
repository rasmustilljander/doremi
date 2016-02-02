#include <Internal/LoggingModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <iostream>
#include <Windows.h>

namespace DoremiEngine
{
    namespace Logging
    {
        LoggingModuleImplementation::LoggingModuleImplementation() : m_submoduleManager(nullptr) {}

        LoggingModuleImplementation::~LoggingModuleImplementation() {}

        void LoggingModuleImplementation::Startup() {}

        void LoggingModuleImplementation::Shutdown() {}
        SubmoduleManager& LoggingModuleImplementation::GetSubModuleManager() { return *m_submoduleManager; }
    }
}

DoremiEngine::Logging::LoggingModule* CreateLoggingModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Logging::LoggingModule* logger = new DoremiEngine::Logging::LoggingModuleImplementation();
    return logger;
}