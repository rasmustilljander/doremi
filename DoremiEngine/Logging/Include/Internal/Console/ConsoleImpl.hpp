#pragma once
#include <Console/Console.hpp>
#include <Console/ConsoleColor.hpp>

#include <string>
#include <map>

namespace DoremiEngine
{
    namespace Logging
    {
        enum class LogTag;

        class ConsoleImpl : public Console
        {
        public:
            /**
            TODO docs
            */
            ConsoleImpl(const std::string& p_consoleName, LogTag p_logTag, const ConsoleColor& p_textColor, const ConsoleColor& p_backgroundColor);

            /**
            TODO docs
            */
            virtual ~ConsoleImpl();

            /**
                The actual method called when calling LogText
            */
            void WriteToConsole(const LogLevel& p_vLevel, const char* p_format, ...) override;

        private:
            std::string m_consoleName;
            ConsoleColor m_textColor, m_backgroundColor;
        };
    }
}
