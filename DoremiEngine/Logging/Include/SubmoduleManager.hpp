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
            virtual ~SubmoduleManager() {}

            /**
            Fetches the main application logger.
            */
            virtual Logger& GetLogger() const = 0;

            /**
            Fetches the consolemanager which manages theoretical external consoles.
            */
            virtual ConsoleManager& GetConsoleManager() const = 0;
        };
    }
}