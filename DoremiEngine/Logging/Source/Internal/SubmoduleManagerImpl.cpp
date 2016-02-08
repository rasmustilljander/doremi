#include <Internal/SubmoduleManagerImpl.hpp>
#include <Internal/Logger/LoggerImpl.hpp>
#include <Internal/Console/ConsoleManagerImpl.hpp>
#include <Internal/LoggingModuleImplementation.hpp>

namespace DoremiEngine
{
    namespace Logging
    {
        SubmoduleManagerImpl::SubmoduleManagerImpl() : m_logger(nullptr), m_consoleManager(nullptr), m_loggingModule(nullptr)
        {
            m_consoleManager = new ConsoleManagerImpl();
        }

        SubmoduleManagerImpl::~SubmoduleManagerImpl()
        {
            if(m_logger != nullptr)
            {
                delete m_logger;
            }

            if(m_consoleManager != nullptr)
            {
                delete m_consoleManager;
            }

            if(m_loggingModule != nullptr)
            {
                delete m_loggingModule;
            }
        }

        void SubmoduleManagerImpl::Initialize(LoggingModuleImplementation& p_loggingModule)
        {
            m_loggingModule = &p_loggingModule;
            m_logger = new LoggerImpl();
            static_cast<LoggerImpl*>(m_logger)->Initialize(m_loggingModule->GetEngineSharedContext().GetWorkingDirectory());
        }

        Logger& SubmoduleManagerImpl::GetLogger() const { return *m_logger; }

        ConsoleManager& SubmoduleManagerImpl::GetConsoleManager() const { return *m_consoleManager; }
    }
}