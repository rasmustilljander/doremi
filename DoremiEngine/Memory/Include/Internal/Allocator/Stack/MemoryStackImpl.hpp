#pragma once

#include <Interface/Allocator/Stack/MemoryStack.hpp>

namespace DoremiEngine
{
    namespace Memory
    {
        class MemoryStackImpl : public MemoryStack
        {
            MemoryStackImpl();
            virtual ~MemoryStackImpl();
        };
    }
}