#pragma once
#include <Utility/Utilities/Include/Memory/Stack/MemoryMarker.hpp>
#include <Utility/Utilities/Include/Memory/MemoryAllocator.hpp>

namespace Doremi
{
    namespace Utilities
    {
        namespace Memory
        {
            class StackAllocator : public MemoryAllocator
            {
            public:
                /**
                    Constructor
                */
                StackAllocator();

                /**
                    Initializes the stack allocator with a given memorysize. Should not be called twice.
                */
                void Initialize(const size_t& p_memorySize);

                /**
                Allocates and returns a pointer to a memory chunk with the size of T.
                Executes the default constructor.
                */
                template <typename T> T* Allocate(const uint8_t& p_alignment = 4)
                {
                    T* object = static_cast<T*>(AllocateAligned(sizeof(T), p_alignment));
                    *object = T();
                    return object;
                }

                /**
                    Clears the entire stack.
                */
                void Clear() override;

                /**
                    Get a marker which can be used to rollback memory.
                */
                MemoryMarker GetMarker();

                /**
                    Rollback stack to a specific location in the stack.
                */
                void FreeToMarker(const MemoryMarker& p_marker);

            protected:
                /**
                The current top of the stack, aka, the currently first available memory in this stack.
                */
                void* m_top;

                void* AllocateAligned(const size_t& p_memorySize, const uint8_t& p_alignment);
                void* AllocateUnaligned(const size_t& p_newMemorySize);
            };
        }
    }
}