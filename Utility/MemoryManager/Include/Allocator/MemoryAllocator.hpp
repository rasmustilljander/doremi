#pragma once
#include <memory>

namespace Utility
{
    namespace MemoryManager
    {
        class MemoryAllocator
        {
            public:
            // TODORT put in pp and remove from public interface
            virtual ~MemoryAllocator() { std::free(m_raw); }

            /**
             Get the current free memory, in bytes, of the allocator.
             */
            const size_t& GetFreeMemory() const { return m_totalMemory - m_occupiedMemory; }

            /**
            Get the current occupied memory, in bytes, of the allocator.
            */
            const size_t& GetOccupiedMemory() const { return m_occupiedMemory; }

            /**
            Get the total memory that the allocator has available (Free + Occupied), in bytes.
            */
            const size_t& GetTotalMemory() const { return m_totalMemory; }

            protected:
            size_t m_occupiedMemory;
            size_t m_totalMemory;
            void* m_raw;
            uint8_t m_alignment;
            bool m_threadShared;

            // TODORT can be moved to cpp
            virtual void Initialize(const size_t& p_memorySize, const uint8_t& p_alignment = 4, const bool& p_threadShared = false)
            {
                m_alignment = p_alignment;
                m_threadShared = p_threadShared;
                m_totalMemory = p_memorySize + m_alignment;
                m_raw = std::malloc(m_totalMemory);
            }
            // TODORT can be moved to cpp

            virtual void Initialize(const size_t& p_memorySize, MemoryAllocator& p_applicationAllocator, const uint8_t& p_alignment = 4,
                                    const bool& p_threadShared = false)
            {
                m_alignment = p_alignment;
                m_threadShared = p_threadShared;
                m_totalMemory = p_memorySize + m_alignment;
                m_raw = p_applicationAllocator.Allocate(m_totalMemory);
            }

            virtual void* Allocate(const size_t& p_memorySize) = 0;

            // TODORT can be moved to cpp
            MemoryAllocator() : m_occupiedMemory(0), m_totalMemory(0), m_raw(nullptr), m_alignment(0), m_threadShared(false) {}
        };
    }
}