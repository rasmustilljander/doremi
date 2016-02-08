#pragma once
#include <cstdint>
namespace Doremi
{
    namespace Utilities
    {
        namespace Memory
        {
            // Hopefully started byte will never be this to begin with...
            static uint8_t STARTED = 241;

            struct StaticData
            {
                StaticData()
                {
                    currentTailOffset = 0;
                    currentHeadOffset = 0;
                    currentObjectCount = 0;
                    started = STARTED;
                }
                size_t currentTailOffset;
                size_t currentHeadOffset;
                size_t currentObjectCount;
                uint8_t started;
            };
        }
    }
}