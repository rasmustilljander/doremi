#pragma once
#include <Utility/MemoryManager/Include/Allocator/MemoryAllocator.hpp>
#include <exception>

namespace Utility
{
    namespace MemoryManager
    {
        class ApplicationAllocator;

        template <typename T> class PoolAllocator : public MemoryAllocator
        {

            PoolAllocator::PoolAllocator() {}

            /**
            Start the pool and allocate their own memory
            */
            void Initialize(const size_t& p_memorySize, MemoryAllocator& p_applicationAllocator, const uint8_t& p_alignment, const bool& p_threadShared)
            {
                // TODORT move to cpp
                MemoryAllocator::Initialize(p_memorySize, p_alignment, p_threadShared);
            }

            /**
            Start the pool with our application allocator as base, distributes already allocated memory from "above".
            */
            void Initialize(const size_t& p_memorySize, MemoryAllocator& p_applicationAllocator, const uint8_t& p_alignment, const bool& p_threadShared)
            {
                // TODORT move to cpp
                MemoryAllocator::Initialize(p_memorySize, p_applicationAllocator, p_alignment, p_threadShared);
            }

            /**
                TODORT docs
            */
            void RawAllocator::InternalInitialize()
            {
                m_firstFree = m_raw;
                if(m_alignment > m_headerSizeInBytes)
                {
                    // TODORT fix freelist
                }
                else
                {
                    // TODORT
                    throw std::runtime_error("m_alignment must be larger than m_headerSizeInBytes:" + m_headerSizeInBytes);
                }
            }

            // void IncreasePoolSize(){} //TODORT
            // void GetPossibleMemoryLeaks
            // void GetOccupiedBlocks (Iterate through all blocks and check flags)

            virtual PoolAllocator::~PoolAllocator() {}

            T* Allocate()
            {
                if((m_numberOfTotalBlocks - m_numberOfOccupiedBlocks) > 0)
                {
                    // Compute adjustment
                    const uint8_t mask = m_alignment - m_headerSizeInBytes;
                    const uint8_t misalignment = (reinterpret_cast<size_t>(m_raw) & mask);
                    const uint8_t adjustment = m_alignment - misalignment;

                    // Get next pointer

                    // Compute

                    // Compute start
                    m_currentFree = reinterpret_cast<size_t*>(reinterpret_cast<size_t>(m_currentFree) + adjustment);

                    // Set adjustment metadata
                    size_t* adjustmentMetaData = reinterpret_cast<size_t*>(reinterpret_cast<size_t>(m_currentFree) - m_headerSizeInBytes);
                    AllocationHeaderBuilder::SetByte(adjustmentMetaData, adjustment);

                    // Set adjustment metadata
                    size_t* adjustmentMetaData = reinterpret_cast<size_t*>(reinterpret_cast<size_t>(m_currentFree) - m_headerSizeInBytes + 1);
                    AllocationHeaderBuilder::SetByte(adjustmentMetaData, adjustment);


                    T* current = m_firstFree;
                    m_firstFree = reinterpret_cast<size_t*>(reinterpret_cast<size_t>(m_firstFree) + sizeof(T) + m_alignment);
                    // TODORT alignment on each segment?
                    ++m_numberOfOccupiedBlocks;
                }
                else
                {
                    // TODORT logging
                    // TODORT could maybe return null instead, dangerous to throw exception if this is supposed to be threadsafe and the exceptions is
                    // not handled corretly
                    throw std::exception("pool is full"); // TODORT better message
                }
            }

            T* Free(T* t)
            {
                delete t;
                ++m_numberOfOccupiedBlocks;
            }

            private:
            void* m_start;
            T* m_firstFree;
            T* m_lastFree;

            bool m_shared;
            size_t m_numberOfOccupiedBlocks;

            // Not sure about
            uint8_t m_alignment;
            uint8_t m_headerSizeInBytes = 3; // [next, flags, adjustment] data
        };
    }
}