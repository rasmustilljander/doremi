#pragma once
#include <Utility/Utilities/Include/Logging/LogTag.hpp>
#include <Utility/Utilities/Include/Logging/LogLevel.hpp>
#include <string>

namespace DoremiEngine
{
    namespace Logging
    {
        class Logger
        {
        public:
            /**
                The actual method called when calling LogText.
                This method is called indirectly via the #define hack below
                An example how to call this correct via indirection is:
                "Logger.LogText(LogTag::Level, LogLevel::Level, "formatstring %d %d", 1, 2);"

                If this metod is called directly the call should looke something like:
                Logger.LT(__FUNCTION__, __LINE__, LogTag::Level, LogLevel::Level, "formatstring %d %d", 1, 2);"
            */
            virtual void LT(const std::string& p_function, const size_t& p_line, const Doremi::Utilities::Logging::LogTag& p_tag,
                            const Doremi::Utilities::Logging::LogLevel& p_vLevel, const char* p_format, ...) = 0;
        };
    }
}

#ifndef LogText
/*
A bit of a hack.
The hack makes it slightly harder to log things, probably introduces non-portable code and maybe adds a small overhead. However,

TODORT Add possibility to turn hack off, then this method should reference another method

*/
#define LogText(...) LT(__FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif