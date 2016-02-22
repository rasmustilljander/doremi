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

            struct TextMetaData
            {
                TextMetaData(LogTag p_logTag, LogLevel p_logLevel, uint16_t p_line, uint16_t p_functionLength, uint16_t p_messageLength)
                    : logTag(p_logTag), logLevel(p_logLevel), line(p_line), functionLength(p_functionLength), messageLength(p_messageLength)
                {
                }
                LogTag logTag;
                LogLevel logLevel;
                uint16_t line;
                uint16_t functionLength;
                uint16_t messageLength;
            };
        }
    }
}