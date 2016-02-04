#pragma once

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