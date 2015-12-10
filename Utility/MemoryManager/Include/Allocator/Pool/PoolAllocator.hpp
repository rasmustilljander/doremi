#pragma once
#include <Utility/MemoryManager/Include/Allocator/MemoryAllocator.hpp>
#include <Utility/MemoryManager/Include/Implentation/Allocator/Pool/PoolAllocatorImpl.hpp>A

namespace Utility
{
    namespace Memory
    {
        template <typename T> class PoolAllocator : public MemoryAllocator
        {
            private:
            PoolAllocator();
            virtual ~PoolAllocator();
        };
    }
}