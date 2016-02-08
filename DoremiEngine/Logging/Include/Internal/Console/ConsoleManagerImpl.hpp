#pragma once
#include <DoremiEngine/Logging/Include/Console/ConsoleManager.hpp>

#include <unordered_map>
#include <string>

namespace Doremi
{
    namespace Utilities
    {
        namespace Logging
        {
            enum class LogTag;
        }
    }
}

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

            /**
                Initializes the consolemanager with a given logger.
            */
            void Initialize(Logger& p_logger);

            /**
                Create a new console with the given parameters, returns an already existing console if one exists with the given name.
            */
            Console& CreateNewConsole(const std::string& p_consoleName, const Doremi::Utilities::Logging::LogTag& p_logtag,
                                      const ConsoleColor& p_textColor, const ConsoleColor& p_backgroundColor) override;

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