#include <Internal/Console/ConsoleImpl.hpp>
#include <Internal/VA_LISTHelper.hpp>
#include <iostream>
#include <Internal/Logger/LogginData.hpp>

namespace DoremiEngine
{
    namespace Logging
    {
        ConsoleImpl::ConsoleImpl(const std::string& p_consoleName, LogTag p_logTag, const ConsoleColor& p_textColor, const ConsoleColor& p_backgroundColor)
            : m_consoleName(p_consoleName), m_textColor(p_textColor), m_backgroundColor(p_backgroundColor)
        {
        }

        ConsoleImpl::~ConsoleImpl() {}

        void ConsoleImpl::WriteToConsole(const LogLevel& p_vLevel, const char* p_format, ...)
        {
            // TODORT Fix so it uses correct structure
            // TODOLOG
            va_list args;
            va_start(args, p_format);
            LoggingData* threadData = new LoggingData();
            threadData->logLevel = p_vLevel;
            toString(threadData->p_message, p_format, args);
            va_end(args);
        }
    }
}