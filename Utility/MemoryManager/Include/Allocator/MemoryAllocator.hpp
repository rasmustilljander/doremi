#pragma once
#include <Utility/MemoryManager/Include/MemorySpecification.hpp>
#include <memory>

namespace Utility
{
    namespace MemoryManager
    {
        class MemoryAllocator
        {
            public:
            MemoryAllocator() : m_occupiedMemory(0), m_totalMemory(0), m_raw(nullptr), m_alignment(0), m_memoryParent(nullptr) {}

            /**
                TODORT docs
            */
            virtual void Clear() = 0;

            /**
                TODORT docs
            */
            // TODORT put in pp and remove from public interface
            virtual ~MemoryAllocator()
            {
                if(m_memoryParent != nullptr)
                {
                    m_memoryParent->Clear();
                }
                else
                {
                    std::free(m_raw);
                }
            }

            /**
                Gets the some specifications about this memorystack.
            */
            // TODORT can be moved to cpp
            MemorySpecification GetMemorySpecification()
            {
                return MemorySpecification(m_totalMemory - m_occupiedMemory, m_totalMemory, m_occupiedMemory);
            }

            protected:
            virtual void* AllocateUnaligned(const size_t& p_memorySize) = 0;
            virtual void* AllocateAligned(const size_t& p_memorySize, const uint8_t& p_alignment) = 0;

            // TODORT can be moved to cpp
            virtual void Initialize(const size_t& p_memorySize, const uint8_t& p_alignment)
            {
                m_alignment = p_alignment;
                m_totalMemory = p_memorySize + m_alignment;
                AllocateFirstTime();
            }

            // TODORT can be moved to cpp
            virtual void Initialize(const size_t& p_memorySize, const uint8_t& p_alignment, MemoryAllocator& p_memoryAllocator)
            {
                m_alignment = p_alignment;
                m_totalMemory = p_memorySize + m_alignment;
                m_memoryParent = &p_memoryAllocator;
                AllocateFirstTime();
            }

            // TODORT can be moved to cpp
            void AllocateFirstTime()
            {
                if(m_memoryParent == nullptr)
                {
                    m_raw = std::malloc(m_totalMemory);
                }
                else
                {
                    if(m_alignment == 0)
                    {
                        m_raw = m_memoryParent->AllocateUnaligned(m_totalMemory);
                    }
                    else
                    {
                        m_raw = m_memoryParent->AllocateAligned(m_totalMemory, m_alignment);
                    }
                }
                memset(m_raw, 0, m_totalMemory); // TODORT could be removed in the future
            }

            // Variables
            size_t m_occupiedMemory;
            size_t m_totalMemory;
            void* m_raw;
            uint8_t m_alignment;
            MemoryAllocator* m_memoryParent;
        };
    }
}