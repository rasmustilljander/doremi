#pragma once

#include <unordered_map>
#include <string>
#include <Utility/DebugLog/Include/VirtualConsole.hpp>
namespace Utility
{
    namespace DebugLog
    {
        class ConsoleManager
        {
            public:
            /**
            TODORT docs
            */
            static void Startup();

            /**
            TODORT docs
            */
            static void Shutdown();

            /**
            TODORT docs
            */
            static ConsoleManager& GetInstance();

            /**
            TODORT docs
            */
            ConsoleManager(ConsoleManager const&) = delete;

            /**
            TODORT docs
            */
            void operator=(ConsoleManager const&) = delete;

            VirtualConsole& CreateNewConsole(const std::string& p_consoleName = "standard");

            private:
            // Static
            static ConsoleManager* m_instance;

            // Nonstatic
            ConsoleManager();
            virtual ~ConsoleManager();

            std::unordered_map<std::string, VirtualConsole*> m_console;
        };
    }
}