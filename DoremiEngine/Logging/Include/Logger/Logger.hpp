#pragma once
#include <DoremiEngine/Logging/Include/Logger/LogTag.hpp>
#include <DoremiEngine/Logging/Include/Logger/LogLevel.hpp>
#include <string>

namespace DoremiEngine
{
    namespace Logging
    {
        class Logger
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

#ifndef LogText
/*
A bit of a hack
*/
#define LogText(...) LT(__FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif