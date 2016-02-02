#include <Internal/SubmoduleManagerImpl.hpp>
#include <Internal/Logger/LoggerImpl.hpp>
#include <Internal/Console/ConsoleManagerImpl.hpp>

namespace DoremiEngine
{
    namespace Logging
    {
        SubmoduleManagerImpl::SubmoduleManagerImpl() : m_logger(nullptr), m_consoleManager(nullptr)
        {
            m_logger = new LoggerImpl();
            m_consoleManager = new ConsoleManagerImpl();
        }

        SubmoduleManagerImpl::~SubmoduleManagerImpl() {}

        Logger& SubmoduleManagerImpl::GetLogger() { return *m_logger; }

        ConsoleManager& SubmoduleManagerImpl::GetConsoleManager() { return *m_consoleManager; }
    }
}