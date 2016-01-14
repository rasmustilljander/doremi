#pragma once
#include <Utility/MemoryManager/Include/Allocator/MemoryAllocator.hpp>
#include <exception>
#include <cstdint>

namespace Utility
{
    namespace MemoryManager
    {
        template <typename T> class FixedSizePoolAllocator : public MemoryAllocator
        {
            public:
            /**
                TODORT docs
            */
            FixedSizePoolAllocator() : m_maxOjectCount(0), m_currentObjectCount(0), m_currentFree(nullptr), MemoryAllocator() {}

            /**
            Start the pool and allocate it's own memory
            */
            void Initialize(const size_t& p_MaxOjectCount, const uint8_t& p_alignment) override
            {
                m_maxOjectCount = p_MaxOjectCount;
                MemoryAllocator::Initialize(m_maxOjectCount * sizeof(T), p_alignment);
                Clear();
            }

            /**
            Start the pool with our application allocator as base, distributes already allocated memory from "above".
            */
            void Initialize(const size_t& p_MaxOjectCount, const uint8_t& p_alignment, MemoryAllocator& p_applicationAllocator)
            {
                m_maxOjectCount = p_MaxOjectCount;
                MemoryAllocator::Initialize(m_maxOjectCount * sizeof(T), p_alignment, p_applicationAllocator);
                Clear();
            }

            /**
            TODORT docs
            */
            T* Allocate()
            {
                if(m_maxOjectCount > m_currentObjectCount)
                {
                    // Save the pointer to return
                    T* returnPointer = static_cast<T*>(m_currentFree);

                    // Set the next free to be the current value it was pointing to
                    m_currentFree = reinterpret_cast<void*>(*(reinterpret_cast<size_t*>(m_currentFree)));

                    // Increase the memory count
                    ++m_currentObjectCount;
                    m_occupiedMemory += sizeof(T);

                    // Run default constructor for the object
                    *returnPointer = T();
                    return returnPointer;
                }
                else
                {
                    // TODORT log
                    throw std::runtime_error("No more space in the pool.");
                }
            }

            /**
            TODORT docs
            */
            void Free(T* p_pointer)
            {
                // TODORT Check to see that the pointer is inside this pool?
                void* prevCurrentFree = m_currentFree;

                // Set the newly created released pointer to be the first free
                m_currentFree = reinterpret_cast<void*>(p_pointer);

                // Set the value of the next
                *reinterpret_cast<size_t*>(m_currentFree) = reinterpret_cast<size_t>(prevCurrentFree);
                --m_currentObjectCount;
                m_occupiedMemory -= sizeof(T);
            }

            /**
            TODORT docs
            */
            void Clear() override
            {
                m_currentObjectCount = 0;
                m_occupiedMemory = 0;
                m_currentFree = m_raw;
                PrepareBlocks();
            }

            protected:
            void PrepareBlocks()
            {
                size_t* current = reinterpret_cast<size_t*>(m_currentFree);
                for(size_t i = 0; i < m_maxOjectCount; ++i)
                {
                    // Compute the location of the next pointer
                    const size_t next = reinterpret_cast<size_t>(current) + sizeof(T);

                    // Save the value of the next pointer as an integer
                    *current = next;

                    // Move current to the next pointer.
                    current = reinterpret_cast<size_t*>(next);
                }
            }

            void* AllocateUnaligned(const size_t& p_memorySize)
            {
                throw std::runtime_error("Cannot allocate arbitrary sized chunks within a FixedSizedPoolAllocator.");
            }

            void* AllocateAligned(const size_t& p_memorySize, const uint8_t& p_alignment)
            {
                throw std::runtime_error("Cannot allocate arbitrary sized chunks within a FixedSizedPoolAllocator.");
            }

            size_t m_maxOjectCount;
            size_t m_currentObjectCount;
            void* m_currentFree;
        };
    }
}