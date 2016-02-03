#pragma once

namespace DoremiEngine
{
    namespace Logging
    {
        class Logger;
        class ConsoleManager;

        class SubmoduleManager
        {
        public:
            /**
            Fetches the main application logger.
            */
            virtual Logger& GetLogger() = 0;

            /**
            Fetches the consolemanager which manages theoretical external consoles.
            */
            virtual ConsoleManager& GetConsoleManager() = 0;
        };
    }
}