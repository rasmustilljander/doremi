#pragma once
#include <Utility/MemoryManager/Include/Allocator/MemoryAllocator.hpp>
#include <Utility/MemoryManager/Include/Allocator/Stack/MemoryMarker.hpp>
#include <Utility/MemoryManager/Include/MemorySpecification.hpp>
#include <cstdint>

namespace Utility
{
    namespace MemoryManager
    {
        class StackAllocator
        {
            public:
            /**
                Constructor
            */
            StackAllocator();

            /**
                TODORT docs
            */
            void Initialize(const size_t& p_memorySize);

            /**
                Destructor
            */
            virtual ~StackAllocator();

            /**
            TODORT docs
            */
            template <typename T> T* Allocate(const uint8_t& p_alignment = 4)
            {
                T* object = static_cast<T*>(Allocate(sizeof(T), p_alignment));
                *object = T();
                return object;
            }

            /**
                Clears the entire stack
            */
            void Clear();

            /**
                Get a marker which can be used to rollback memory.
            */
            MemoryMarker GetMarker();

            /**
                Rollback stack to a specific location in the stack.
            */
            void FreeToMarker(const MemoryMarker& p_marker);

            /**
                Gets the some specifications about this memorystack.
            */
            MemorySpecification GetMemorySpecification();

            protected:
            void* m_raw;
            void* m_top;
            size_t m_memorySize;
            size_t m_allocated;

            void* Allocate(const size_t& p_memorySize, const uint8_t& p_alignment);
            void* AllocateUnaligned(const size_t& p_newMemorySize);
            uint8_t ComputeAdjustment(void* p_adress, const uint8_t& p_alignment);
        };
    }
}