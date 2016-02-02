#include <Internal/LoggingModuleImplementation.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Internal/SubmoduleManagerImpl.hpp>

namespace DoremiEngine
{
    namespace Logging
    {
        LoggingModuleImplementation::LoggingModuleImplementation() : m_submoduleManager(nullptr) {}

        LoggingModuleImplementation::~LoggingModuleImplementation() {}

        void LoggingModuleImplementation::Startup() { m_submoduleManager = new SubmoduleManagerImpl(); }

        void LoggingModuleImplementation::Shutdown() {}
        SubmoduleManager& LoggingModuleImplementation::GetSubModuleManager() { return *m_submoduleManager; }
    }
}

DoremiEngine::Logging::LoggingModule* CreateLoggingModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Logging::LoggingModule* logger = new DoremiEngine::Logging::LoggingModuleImplementation();
    return logger;
}