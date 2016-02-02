#include <Internal/Console/ConsoleManagerImpl.hpp>
#include <Internal/Console/ConsoleImpl.hpp>

namespace DoremiEngine
{
    namespace Logging
    {
        ConsoleManagerImpl::ConsoleManagerImpl() {}

        ConsoleManagerImpl::~ConsoleManagerImpl()
        {
            for(auto i : m_console)
            {
                delete i.second;
            }
            m_console.clear();
        }

        void ConsoleManagerImpl::Initialize(Logger& p_logger) { m_logger = &p_logger; }


        Console& ConsoleManagerImpl::CreateNewConsole(const std::string& p_consoleName, const LogTag& p_logtag, const ConsoleColor& p_textColor,
                                                      const ConsoleColor& p_backgroundColor)
        {
            if(m_console.count(p_consoleName) == 0)
            {
                ConsoleImpl* console = new ConsoleImpl(p_consoleName, p_logtag, p_textColor, p_backgroundColor);
                m_console.insert(std::pair<std::string, Console*>(p_consoleName, console));
            }
            else
            {
                // found an key, do nothing special
            }
            return *m_console[p_consoleName];
        }

        Console& ConsoleManagerImpl::GetConsole(const std::string& p_consoleName)
        {
            if(m_console.count(p_consoleName) > 0)
            {
                // found an key, do nothing special
            }
            else
            {
                throw std::runtime_error("Console with: '" + p_consoleName +
                                         "' does not exist."); // TODOXX Cannot be thrown correctly across dll bounderis
            }
            return *m_console[p_consoleName];
        }
    }
}