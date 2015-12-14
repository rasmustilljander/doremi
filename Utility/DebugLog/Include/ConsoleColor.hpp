#pragma once

namespace Utility
{
    namespace DebugLog
    {
        enum class ConsoleColorEnum : size_t
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
            ConsoleColor() { state = ConsoleColorEnum::BLACK; }

            ConsoleColor(ConsoleColorEnum p_state) { state = p_state; }

            ConsoleColor(size_t p_value) { stateValue = p_value; }
            ConsoleColorEnum state;
            size_t stateValue;
        };
    }
}
