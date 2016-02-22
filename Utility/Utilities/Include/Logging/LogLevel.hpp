#pragma once

#include <cstdint>

namespace Doremi
{
    namespace Utilities
    {
        namespace Logging
        {
            enum class LogLevel : uint8_t
            {
                FATAL_ERROR, // Error in which the game is not expected to return from.
                NON_FATAL_ERROR, // Error which the game can return from
                WARNING, // Warns for potential problems
                DEBUG, // Debug level
                INFO, // Info level, everything from init, startup and delete
                MASS_DATA_PRINT, // Indicate large amounts of data, can be handles differently
                NOLEVEL // No level
            };
        }
    }
}
