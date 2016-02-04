#pragma once
#include <Utility/Utilities/Include/Memory/MemoryAllocator.hpp>
#include <exception>
#include <cstdint>

namespace Doremi
{
    namespace Utilities
    {
        namespace Memory
        {
            template <typename T> class FixedSizePoolAllocator : public MemoryAllocator
            {
            public:
                /**
                    Constructpr
                */
                FixedSizePoolAllocator() : m_maxOjectCount(0), m_currentObjectCount(0), m_currentFree(nullptr), MemoryAllocator() {}

                /**
                Start the pool and allocate it's own memory
                */
                void Initialize(const size_t& p_MaxOjectCount, const uint8_t& p_alignment)
                {
                    m_maxOjectCount = p_MaxOjectCount;
                    MemoryAllocator::Initialize(m_maxOjectCount * sizeof(T), p_alignment);
                    Clear();
                }

                /**
                Allocates an object of T and runs the default constructor.
                Returns a pointer to the newly created object.
                Throws exception allocation was not possible.
                */
                T* Allocate()
                {
                    if(m_maxOjectCount > m_currentObjectCount)
                    {
                        // Save the pointer to return
                        T* returnPointer = static_cast<T*>(m_currentFree);

                        // Set the next free to be the current value it was pointing to
                        size_t value = *reinterpret_cast<size_t*>(m_currentFree);
                        m_currentFree = reinterpret_cast<void*>(value);

                        // Increase the memory count
                        ++m_currentObjectCount;
                        m_occupiedMemory += sizeof(T);

                        // Run default constructor for the object
                        *returnPointer = T();
                        return returnPointer;
                    }
                    else
                    {
                        // TODOLOG
                        // TODORT log
                        // TODOXX Exception may be problematic as it cannot be thrown across dll borders.
                        throw std::runtime_error("No more space in the pool.");
                    }
                }

                /**
                Attempts to free a pointer from the pool. Does NOT
                Throws an exception if the pointer is not contained within this pool.
                */
                void Free(T* p_pointer)
                {
                    if(AssertAdresstInside(static_cast<void*>(p_pointer)))
                    {
                        void* prevCurrentFree = m_currentFree;

                        // Set the newly created released pointer to be the first free
                        m_currentFree = reinterpret_cast<void*>(p_pointer);

                        // Set the value of the next
                        *static_cast<size_t*>(m_currentFree) = reinterpret_cast<size_t>(prevCurrentFree);

                        // Reduce the object count
                        --m_currentObjectCount;
                        m_occupiedMemory -= sizeof(T);
                    }
                    else
                    {
                        throw std::runtime_error("This memorypool does not contain this adress.");
                    }
                }

                /**
                Clears the entire pool of data, does not execute the constructor on the content.
                // TODORT Execute the destructor on each object ?
                */
                void Clear() override
                {
                    m_currentObjectCount = 0;
                    m_occupiedMemory = 0;
                    m_currentFree = GetAdressStartAligned();
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

                size_t m_maxOjectCount; // Max amount of objects that can be contained within the pool
                size_t m_currentObjectCount; // The current number of objects in the pool
                void* m_currentFree; // Pointer to current free memory
            };
        }
    }
}