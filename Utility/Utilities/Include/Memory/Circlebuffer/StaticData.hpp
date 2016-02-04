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
                    currentTail = nullptr;
                    currentHead = nullptr;
                    currentObjectCount = 0;
                    started = STARTED;
                }
                void* currentTail;
                void* currentHead;
                size_t currentObjectCount;
                uint8_t started;
            };
        }
    }
}