#pragma once

namespace Utility
{
    namespace DebugLog
    {
        enum class LogLevel // TODORT move to hpp
        {
            FATAL_ERROR,
            NON_FATAL_ERROR,
            WARNING,
            SUCCESS,
            DEBUG_PRINT,
            INIT_PRINT,
            START_PRINT,
            PINK_PRINT,
            PACKET_PRINT,
            MASS_DATA_PRINT,
            NOLEVEL,
            HELP_PRINT,
            IDENTIFY_PRINT,
        };
    }
}
