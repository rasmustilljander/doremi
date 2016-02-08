#pragma once
#include <Utility/Utilities/Include/Logging/LogTag.hpp>
#include <Utility/Utilities/Include/Logging/LogLevel.hpp>
#include <Utility/Utilities/Include/Constants/LoggerConstants.hpp>
#include <string>

namespace Doremi
{
    namespace Utilities
    {
        namespace Logging
        {
            struct LogTextData
            {
                char function[Constants::LONGEST_FUNCTION_NAME];
                size_t line;
                char message[Constants::LONGEST_MESSAGE_NAME];
                LogTag logTag;
                LogLevel logLevel;
            };
        }
    }
}