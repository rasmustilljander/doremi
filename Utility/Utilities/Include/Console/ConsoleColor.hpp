#pragma once

#include <cstdint>

namespace Doremi
{
    namespace Utilities
    {
        namespace Console
        {
            enum class ConsoleColorEnum : uint8_t
            {
                BLACK = 0,
                DARK_BLUE = 1,
                DARK_GREEN = 2,
                DARK_AQUA = 3,
                DARK_CYAN = 3,
                DARK_RED = 4,
                DARK_PURPLE = 5,
                DARK_PINK = 5,
                DARK_MANGENTA = 5,
                DARK_YELLOW = 6,
                DARK_WHITE = 7,
                GRAY = 8,
                BLUE = 9,
                GREEN = 10,
                AQUA = 11,
                CYAN = 11,
                RED = 12,
                PURPLE = 13,
                PINK = 13,
                MANGENTA = 13,
                YELLOW = 14,
                WHITE = 15
            };

            union ConsoleColor
            {
                ConsoleColor(ConsoleColorEnum consoleColorEnum) : type(consoleColorEnum) {}
                ConsoleColorEnum type;
                uint8_t value;
            };
        }
    }
}