#pragma once
#include <Utility/MemoryManager/Include/Allocator/MemoryAllocator.hpp>

namespace DoremiEngine
{
    namespace Memory
    {
        template <typename T> class MemoryPool : public MemoryAllocator
        {
        };
    }
}