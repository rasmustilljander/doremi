#pragma once
#include <DirectXMath.h>
using namespace DirectX;
namespace Doremi
{
    namespace Core
    {
        enum class CharacterControlFlags : int
        {
            none,
        };
        /**
        Contains information about a character controller*/
        struct CharacterControlComponent
        {
            // Dimensions of the capsule. X = height, Y = radius
            XMFLOAT2 dims;
            CharacterControlFlags flags = CharacterControlFlags::none;
        };
    }
}