#pragma once

#include <Interface/Allocator/Pool/MemoryPool.hpp>

namespace DoremiEngine
{
    namespace Memory
    {
        class MemoryPoolImpl : public MemoryPool
        {
            MemoryPoolImpl();
            virtual ~MemoryPoolImpl();
        };
    }
}