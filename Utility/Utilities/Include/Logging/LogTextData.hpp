#pragma once
#include <Utility/Utilities/Include/Logging/LogTag.hpp>
#include <Utility/Utilities/Include/Logging/LogLevel.hpp>
#include <string>

namespace Doremi
{
    namespace Utilities
    {
        namespace Logging
        {
            struct LogTextData
            {
                std::string function; // TODORT char*
                size_t line;
                LogTag logTag;
                LogLevel logLevel;
                std::string p_message; // TODORT char*
            };
        }
    }
}