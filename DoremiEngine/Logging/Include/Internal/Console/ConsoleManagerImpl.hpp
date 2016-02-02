#pragma once

#include <DoremiEngine/Logging/Include/Console/ConsoleManager.hpp>

#include <unordered_map>
#include <string>
namespace DoremiEngine
{
    namespace Logging
    {
        class Logger;

        class ConsoleManagerImpl : public ConsoleManager
        {
        public:
            ConsoleManagerImpl();
            virtual ~ConsoleManagerImpl();

            void Initialize(Logger& p_logger);

            /**
                TODORT docs
            */
            Console& CreateNewConsole(const std::string& p_consoleName, const LogTag& p_logtag, const ConsoleColor& p_textColor,
                                      const ConsoleColor& p_backgroundColor) override;

            /**
                Returns an already existing console, throws exception if no console exists.
            */
            Console& GetConsole(const std::string& p_consoleName = "standard") override;

        private:
            std::unordered_map<std::string, Console*> m_console;
            Logger* m_logger;
        };
    }
}