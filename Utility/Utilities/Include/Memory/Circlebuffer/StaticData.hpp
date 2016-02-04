#pragma once
namespace Doremi
{
    namespace Utilities
    {
        namespace Memory
        {
            struct StaticData
            {
                StaticData()
                {
                    currentTail = nullptr;
                    currentHead = nullptr;
                    currentObjectCount = 0;
                }
                void* currentTail;
                void* currentHead;
                size_t currentObjectCount;
            };
        }
    }
}