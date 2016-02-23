#pragma once
#include <cstdint>
namespace Doremi
{
    namespace Utilities
    {
        namespace Memory
        {
            struct ArbitraryStaticData
            {
                ArbitraryStaticData() : currentTailOffset(0), currentHeadOffset(0) {}
                size_t currentTailOffset; // Only changed by the consumer
                size_t currentHeadOffset; // Only changed by the producer
            };
        }
    }
}