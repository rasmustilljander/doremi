#pragma once
#include <Utility/MemoryManager/Include/Allocator/MemoryAllocator.hpp>
#include <Utility/MemoryManager/Include/Implentation/Allocator/Stack/StackAllocatorImpl.hpp>

namespace Utility
{
    namespace Memory
    {
        class StackAllocator : public MemoryAllocator
        {
            private:
            StackAllocator();
            virtual ~StackAllocator();
        };
    }
}