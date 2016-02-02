#include <ConsoleManager.hpp>
#include <Internal/VirtualConsoleImpl.hpp>
#include <Internal/ThreadPool.hpp>

namespace
{
    // TODORT This method of working with threads should be removed later.
    ctpl::thread_pool* g_threadPool;
}

namespace DoremiEngine
{
    namespace Logging
    {
        ConsoleManager* ConsoleManager::m_instance = nullptr;

        void ConsoleManager::Startup()
        {
            if(m_instance == nullptr)
            {
                m_instance = new ConsoleManager();
            }
        }

        void ConsoleManager::Shutdown()
        {
            delete m_instance;
            m_instance = nullptr;
        }

        ConsoleManager& ConsoleManager::GetInstance() { return *m_instance; }

        VirtualConsole& ConsoleManager::CreateNewConsole(const std::string& p_consoleName, bool p_writeToConsole, bool p_writeToFile,
                                                         const ConsoleColor& p_textColor, const ConsoleColor& p_backgroundColor)
        {
            if(m_console.count(p_consoleName) == 0)
            {
                VirtualConsoleImpl* console =
                    new VirtualConsoleImpl(*g_threadPool, p_consoleName, p_writeToConsole, p_writeToFile, p_textColor, p_backgroundColor);
                console->Initialize();
                m_console.insert(std::pair<std::string, VirtualConsole*>(p_consoleName, console));
            }
            else
            {
                // found an key, do nothing special
            }
            return *m_console[p_consoleName];
        }

        VirtualConsole& ConsoleManager::GetConsole(const std::string& p_consoleName)
        {
            if(m_console.count(p_consoleName) > 0)
            {
                // found an key, do nothing special
            }
            else
            {
                throw std::runtime_error("Console with: '" + p_consoleName + "' does not exist.");
            }
            return *m_console[p_consoleName];
        }

        ConsoleManager::ConsoleManager() { g_threadPool = new ctpl::thread_pool(4); }

        ConsoleManager::~ConsoleManager()
        {
            delete g_threadPool;
            for(auto i : m_console)
            {
                delete i.second;
            }
            m_console.clear();
        }
    }
}