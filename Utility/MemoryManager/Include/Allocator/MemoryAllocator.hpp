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
            TODORT docs
            */
            MemoryAllocator();

            /**
            TODORT docs
            */
            virtual ~MemoryAllocator();

            /**
            TODORT docs
            */
            virtual void Clear() = 0;

            /**
                Gets the some specifications about this memorystack.
            */
            MemorySpecification GetMemorySpecification();


            protected:
            void Initialize(const size_t& p_memorySize, const uint8_t& p_alignment);
            void AllocateFirstTime();
            uint8_t ComputeAdjustment(void* p_adress, const uint8_t& p_alignment);
            bool AssertAdresstInside(void* p_adressToAssert) const;

            void* GetAdressStartRaw() const { return m_memoryStartRaw; }
            void* GetAdressEndRaw() const { return m_memoryEndRaw; }
            void* GetAdressStartAligned() const { return m_memoryStartRaw; }

            // Variables
            size_t m_occupiedMemory;
            size_t m_totalMemory;
            uint8_t m_alignment;
            uint8_t m_adjustment;

            private:
            void* m_memoryStartRaw;
            void* m_memoryEndRaw;
            void* m_memoryStartAligned;
        };
    }
}