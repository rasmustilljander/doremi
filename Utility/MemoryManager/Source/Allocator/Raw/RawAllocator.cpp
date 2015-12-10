#include <Allocator/Raw/RawAllocator.hpp>
#include <memory>

namespace Utility
{
    namespace Memory
    {
        RawAllocator::RawAllocator()
            : m_start(nullptr), m_end(nullptr), m_totalMemory(0), m_occupiedMemory(0), m_numBlocks(0), m_alignment(0), m_shared(false)
        {
        }

        void RawAllocator::Initialize(const size_t& p_memorySize, const bool& p_threadShared)
        {
            m_start = (std::malloc(p_memorySize));

            size_t* raw = (size_t*)malloc(p_memorySize + m_alignment);
            size_t mask = m_alignment - 1;

            size_t misalignment = (reinterpret_cast<uint32_t>(raw) & mask);
            size_t adjustment = m_alignment - misalignment;

            m_start = reinterpret_cast<uint32_t*>(reinterpret_cast<uint32_t>(raw) + adjustment);
        }

        RawAllocator::~RawAllocator() { std::free(m_start); }

        size_t RawAllocator::GetFreeMemory() { return m_totalMemory - m_occupiedMemory; }

        size_t RawAllocator::GetOccupiedMemory() { return m_occupiedMemory; }

        size_t RawAllocator::GetTotalMemory() { return m_totalMemory; }
    }
}