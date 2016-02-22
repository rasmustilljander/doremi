#pragma once
#include <Console/Console.hpp>
#include <Console/ConsoleColor.hpp>

#include <string>
#include <map>

namespace Doremi
{
    namespace Utilities
    {
        namespace Logging
        {
            enum class LogTag : uint8_t;
            enum class LogLevel : uint8_t;
        }
    }
}

namespace DoremiEngine
{
    namespace Logging
    {

        class ConsoleImpl : public Console
        {
        public:
            /**
            Constructor
            TODORT
            TODOMEM Convert into empty constructor and add init function for easier memory overwrite.
            */
            ConsoleImpl(const std::string& p_consoleName, Doremi::Utilities::Logging::LogTag p_logTag, const ConsoleColor& p_textColor,
                        const ConsoleColor& p_backgroundColor);

            /**
            Destructor
            */
            virtual ~ConsoleImpl();

            /**
                The actual method called when calling LogText
            */
            void WriteToConsole(const Doremi::Utilities::Logging::LogLevel& p_vLevel, const char* p_format, ...) override;

        private:
            std::string m_consoleName;
            ConsoleColor m_textColor, m_backgroundColor;
        };
    }
}
