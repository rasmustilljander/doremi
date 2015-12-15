#include <ConsoleManager.hpp>
#include <Internal/VirtualConsoleImpl.hpp>
#include <Internal/ThreadPool.hpp>

namespace
{
    ctpl::thread_pool* g_threadPool;
}

namespace Utility
{

    namespace DebugLog
    {
        ConsoleManager* ConsoleManager::m_instance = nullptr;

        void ConsoleManager::Startup()
        {
            if(m_instance == nullptr)
            {
                m_instance = new ConsoleManager();
            }
            // TODORT logging
        }

        void ConsoleManager::Shutdown()
        {
            delete m_instance;
            m_instance = nullptr;
        }

        ConsoleManager& ConsoleManager::GetInstance() { return *m_instance; }

        VirtualConsole& ConsoleManager::CreateNewConsole(const std::string& p_consoleName)
        {
            if(m_console.count(p_consoleName) == 0)
            {
                m_console.insert(std::pair<std::string, VirtualConsole*>(p_consoleName, new VirtualConsoleImpl(p_consoleName, *g_threadPool)));
            }
            else
            {
                // found an key, do nothing special
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