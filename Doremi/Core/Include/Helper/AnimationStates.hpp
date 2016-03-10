#pragma once
// Project specific

// Standard
#include <vector>
#include <DirectXMath.h>
namespace Doremi
{
    namespace Core
    {
        enum class SkeletalAnimationType
        {
            RANGEDENEMY,
            MELEENEMY,
            PLAYER,
            PLAYER2,
        };
        struct AnimationTransitionInfo
        {
            float startTime;
            float elapsedSinceStart;
        };
    }
}
