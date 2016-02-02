#pragma once
#include <Logger/LogTag.hpp>
#include <Logger/LogLevel.hpp>
#include <string>

namespace DoremiEngine
{
    namespace Logging
    {
        struct LoggingData
        {
            std::string function;
            size_t line;
            LogTag logTag;
            LogLevel logLevel;
            std::string p_message;
        };
    }
}