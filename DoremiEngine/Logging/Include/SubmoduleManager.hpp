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
            TODO docs
            */
            virtual Logger& GetLogger() = 0;

            /**
            TODO docs
            */
            virtual ConsoleManager& GetConsoleManager() = 0;
        };
    }
}