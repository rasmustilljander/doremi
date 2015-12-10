#pragma once
#include <Utility/MemoryManager/Include/Allocator/MemoryAllocator.hpp>

namespace Utility
{
    namespace Memory
    {
        class RawAllocator : public MemoryAllocator
        {
            public:
            /**
                Constructor
            */
            RawAllocator();

            /**
                Constructor
            */
            void Initialize(const size_t& p_memorySize, const bool& p_threadShared = false);

            /**
                Constructor, takes memorySize in bytes.
            */
            virtual ~RawAllocator();

            /*
            Header
            pointerToNext
            sizeOfCurrent
            */
            /**
                TODORT
            */
            template <typename T>
            // std::unique_ptr<T> Allocate()
            T* Allocate()
            {
                T* a = new T();
                // return a;
                std::shared_ptr<T> b = std::make_shared<T>(a);
                return b.get();
            }

            /**
                TODORT
            */
            template <typename T> void Free(T* t) {}

            /**
            Get the current free memory, in bytes, of the allocator.
            */
            size_t GetFreeMemory() override;

            /**
            Get the current occupied memory, in bytes, of the allocator.
            */
            size_t GetOccupiedMemory() override;

            /**
            Get the total memory that the allocator has available (Free + Occupied), in bytes.
            */
            size_t GetTotalMemory() override;

            private:
            void* m_start;
            void* m_end; // Not sure about this;

            size_t m_totalMemory;
            size_t m_occupiedMemory;
            size_t m_numBlocks;

            bool m_shared;

            // Not sure about
            size_t m_alignment;

            // Will probably need this
        };
    }
}