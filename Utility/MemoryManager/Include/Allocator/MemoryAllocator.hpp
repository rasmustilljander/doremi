#pragma once
#include <Utility/MemoryManager/Include/MemorySpecification.hpp>
#include <cstdint>

namespace Utility
{
    namespace MemoryManager
    {
        class MemoryAllocator
        {
        public:
            /**
            Constructor
            */
            MemoryAllocator();

            /**
            Destructor
            */
            virtual ~MemoryAllocator();

            /**
            Clears the allocator, does -NOT- call any destructors.
            */
            virtual void Clear() = 0;

            /**
                Gets the some specifications about this memorystack.
            */
            MemorySpecification GetMemorySpecification();


        protected:
            void Initialize(const size_t& p_memorySize, const uint8_t& p_alignment);
            void AllocateFirstTime();

            /**
                Computes an adjustment for a given pointer and alignment.
                The alignment must be non-zero and contained in the set of 2^x, the functions return zero otherwise.
            */
            static uint8_t ComputeAdjustment(void* p_adress, const uint8_t& p_alignment);
            bool AssertAdresstInside(void* p_adressToAssert) const;

            void* GetAdressStartRaw() const { return m_memoryStartRaw; }
            void* GetAdressEndRaw() const { return m_memoryEndRaw; }
            void* GetAdressStartAligned() const { return m_memoryStartAligned; }

            // Variables
            size_t m_occupiedMemory;
            size_t m_totalMemory;


        private:
            /**
            The raw pointer to the beginning of the memory chunk that this allocator is liable for.
            Used to free the memory in the end.
            Used to assert that other pointers are within the chunk.
            */
            void* m_memoryStartRaw;

            /**
            The raw pointer to the end of the memory chunk that this allocator is liable for.
            Used to assert that other pointers are within the chunk.
            */
            void* m_memoryEndRaw;

            /**
            The aligned pointer top the beginning of the memory chunk that this allocator is liable for.
            The adress that is used as the "start" pointer during allocation.
            */
            void* m_memoryStartAligned;

            /**
            The global alignment for this specific allocator, only used to allocate the entire chunk.
            */
            uint8_t m_alignment;

            /**
            The global adjustment for this specific allocator, only used to allocate the entire chunk.
            */
            uint8_t m_adjustment;
        };
    }
}