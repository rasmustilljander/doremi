#pragma once
// Project specific

// Standard
#include <vector>
#include <DirectXMath.h>
namespace Doremi
{
    namespace Core
    {
        enum class DoremiStates
        {
            RUNGAME = 1,
            OPTIONS = 2,
            EXIT = 4,
            PAUSE = 8,
            MAINMENU = 16,
            VICTORY = 32,
        };
    }
}
