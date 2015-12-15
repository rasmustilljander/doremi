#pragma once
#include <Windows.h>
#include <string>
#include <Utility/DebugLog/Include/ConsoleColor.hpp>
#include <Utility/DebugLog/Include/LogLevel.hpp>
#include <Utility/DebugLog/Include/LogTag.hpp>

namespace Utility
{
    namespace DebugLog
    {
        class VirtualConsole
        {
            public:
            /**
                The initialize method, can be called with no arguments
                // TODORT I'll need an manager for creating consoles, atm it's possible to create multiple identical
            */
            virtual void Initialize(bool p_writeToConsole = true, bool p_writeToFile = true, const ConsoleColor& p_textColor = ConsoleColorEnum::WHITE,
                                    const ConsoleColor& p_backgroundColor = ConsoleColorEnum::BLACK) = 0;

            /**
            The actual method called when calling LogText
            */
            virtual void LT(const std::string& p_function, const size_t& p_line, const LogTag& p_tag, const LogLevel& p_vLevel, const char* p_format, ...) = 0;

            /**
                Dummy method for the intellisense to recognize the method
            */
            void LogText(LogTag p_tag, LogLevel p_vLevel, const char* p_format, ...){};
        };
    }
}
/*
A bit of a hack
*/
#define LogText(...) LT(__FUNCTION__, __LINE__, ##__VA_ARGS__)
