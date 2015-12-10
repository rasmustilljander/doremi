#pragma once
#include <Utility/MemoryManager/Include/Allocator/MemoryAllocator.hpp>
#include <Utility/MemoryManager/Include/Implentation/Allocator/Component/ComponentAllocatorImpl.hpp>

namespace Utility
{
    namespace Memory
    {
        template <typename T> class ComponentAllocator : public MemoryAllocator
        {
            private:
            ComponentAllocator();
            virtual ~ComponentAllocator();
        };
    }
}