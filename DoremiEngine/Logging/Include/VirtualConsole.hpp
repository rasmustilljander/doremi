#pragma once
#include <Windows.h>
#include <string>
#include <DoremiEngine\Logging\Include\ConsoleColor.hpp>
#include <DoremiEngine\Logging\Include\LogLevel.hpp>
#include <DoremiEngine\Logging\Include\LogTag.hpp>

namespace DoremiEngine
{
    namespace Logging
    {
        class VirtualConsole
        {
        public:
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
