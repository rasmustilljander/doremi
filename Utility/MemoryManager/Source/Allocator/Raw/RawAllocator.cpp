#include <Allocator/Raw/RawAllocator.hpp>
#include <memory>
#include <Utility/Utilities/Include/Memory/AllocationHeaderBuilder.hpp>

namespace Utility
{
    namespace MemoryManager
    {
        RawAllocator::RawAllocator() : m_numBlocks(0), m_headerSizeInBytes(2), MemoryAllocator() {}

        void RawAllocator::Initialize(const size_t& p_memorySize, const uint8_t& p_alignment)
        {
            MemoryAllocator::Initialize(p_memorySize, p_alignment);
        }

        RawAllocator::~RawAllocator() {}


        //  void* RawAllocator::Allocate(const size_t& p_memorySize)
        //   {
        /*
        if (m_alignment <= m_headerSizeInBytes)
        {
            // TODORT check for overflow ? for p_alignment
            throw std::runtime_error("Alignment must be bigger than m_headerSizeInBytes");
        }

        // Compute adjustment
        const uint8_t mask = m_alignment - m_headerSizeInBytes;
        const uint8_t misalignment = (reinterpret_cast<size_t>(m_raw) & mask);
        const uint8_t adjustment = m_alignment - misalignment;

        // Compute start
        m_currentFree = reinterpret_cast<size_t*>(reinterpret_cast<size_t>(m_currentFree) + adjustment);


        // Set adjustment metadata
        size_t* adjustmentMetaData = reinterpret_cast<size_t*>(reinterpret_cast<size_t>(m_currentFree) - m_headerSizeInBytes);
        AllocationHeaderBuilder::SetByte(adjustmentMetaData, adjustment);

        // Set adjustment metadata
        size_t* adjustmentMetaData = reinterpret_cast<size_t*>(reinterpret_cast<size_t>(m_currentFree) - m_headerSizeInBytes + 1);
        AllocationHeaderBuilder::SetByte(adjustmentMetaData, adjustment);
        */
        //         return std::malloc(p_memorySize); // TODORT implement correctly
        //     }
        //
        void RawAllocator::Free(void* p_pointer)
        {
            std::free(p_pointer); // TODORT implement correctly
        }
    }
}