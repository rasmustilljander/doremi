#pragma once
#include <cstdint>
namespace Doremi
{
    namespace Utilities
    {
        namespace Memory
        {
            // Hopefully started byte will never be this to begin with...
            static uint8_t ARBITRARY_CIRCLE_BUFFER_STARTED = 241;

            struct ArbitraryStaticData
            {
                ArbitraryStaticData()
                {
                    currentTailOffset = 0;
                    currentHeadOffset = 0;
                    totalMemory = 0;
                    allocatedMemory = 0;
                    started = ARBITRARY_CIRCLE_BUFFER_STARTED;
                }
                size_t currentTailOffset;
                size_t currentHeadOffset;
                size_t totalMemory;
                size_t allocatedMemory;
                uint8_t started;
            };
        }
    }
}