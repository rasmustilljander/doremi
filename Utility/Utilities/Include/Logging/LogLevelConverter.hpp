#pragma once
#include <Utility/Utilities/Include/Logging/LogLevelInfo.hpp>
#include <Utility/Utilities/Include/Logging/LogLevel.hpp>

#include <exception>
#include <map>

namespace Doremi
{
    namespace Utilities
    {
        namespace Logging
        {
            namespace LogLevelConverter
            {
                namespace Internal
                {
                    static std::map<LogLevel, LogLevelInfo> createMap()
                    {
                        std::map<LogLevel, LogLevelInfo> map;
                        map[LogLevel::FATAL_ERROR] = LogLevelInfo("FATAL_ERROR");
                        map[LogLevel::NON_FATAL_ERROR] = LogLevelInfo("NON_FATAL_ERROR");
                        map[LogLevel::WARNING] = LogLevelInfo("WARNING");
                        map[LogLevel::DEBUG] = LogLevelInfo("DEBUG");
                        map[LogLevel::INFO] = LogLevelInfo("INFO");
                        map[LogLevel::MASS_DATA_PRINT] = LogLevelInfo("MASS_DATA_PRINT");
                        map[LogLevel::NOLEVEL] = LogLevelInfo("NOLEVEL");

                        return map;
                    }
                    const std::map<LogLevel, LogLevelInfo>& info = createMap();
                }

                static LogLevelInfo convert(LogLevel p_logLevel)
                {
                    if(Internal::info.count(p_logLevel) == 1)
                    {
                        return Internal::info.at(p_logLevel);
                    }
                    throw std::runtime_error("The given loglevel does not exist.");
                };
            }
        }
    }
}