#pragma once

namespace DoremiEditor
{
    namespace Core
    {
        enum class bitmask : int
        {
            COLORMAP = 0x01,
            GLOWMAP = 0x02,
            SPECMAP = 0x04,
            BUMPMAP = 0x08
        };
    }
}