#pragma once

namespace Utility
{
    namespace Memory
    {
        class MemoryAllocator
        {
            /**
                Get the current free memory, in bytes, of the allocator.
            */
            virtual size_t GetFreeMemory() = 0;

            /**
                Get the current occupied memory, in bytes, of the allocator.
            */
            virtual size_t GetOccupiedMemory() = 0;

            /**
                Get the total memory that the allocator has available (Free + Occupied), in bytes.
            */
            virtual size_t GetTotalMemory() = 0;
        };
    }
}