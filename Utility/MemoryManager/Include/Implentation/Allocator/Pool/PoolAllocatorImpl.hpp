#pragma once
#include <Utility/MemoryManager/Include/Allocator/MemoryAllocator.hpp>

namespace Utility
{
    namespace Memory
    {
        template <typename T> PoolAllocator::PoolAllocator() {}

        template <typename T> virtual PoolAllocator::~PoolAllocator() {}
    }
}