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
                    : currentTailOffset(0), currentHeadOffset(0), totalMemoryInBytes(0), totalConsumedMemoryInBytes(0), totalProducedMemoryInBytes(0), started(0)
                {
                    started = ARBITRARY_CIRCLE_BUFFER_STARTED;
                }
                uint32_t currentTailOffset; // Only changed by the consumer
                uint32_t currentHeadOffset; // Only changed by the producer
                uint32_t totalMemoryInBytes; // Should not be changed
                uint32_t totalConsumedMemoryInBytes; // Only changed by the consumer, but will be reset at some point.
                uint32_t totalProducedMemoryInBytes; // Only changed by the producer, but will be reset at some point.
                uint8_t started;
            };
        }
    }
}