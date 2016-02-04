#pragma once
#include <Utility/Utilities/Include/Memory/MemoryAllocator.hpp>
#include <cstdint>

namespace Utility
{
    namespace MemoryManager
    {
        class RawAllocator : public Doremi::Utilities::Memory::MemoryAllocator
        {
        public:
            /**
                Constructor
            */
            RawAllocator();

            /**
                TODORT docs
            */
            virtual void Initialize(const size_t& p_memorySize, const uint8_t& p_alignment);

            /**
                Destructor
            */
            virtual ~RawAllocator();

            /**
            TODORT docs
            */
            //    void* Allocate(const size_t& p_memorySize) override;

            /**
            TODORT docs
            */
            void Free(void* p_pointer);

            /**
                TODORT docs
            */
            template <typename T> T* Allocate(const uint8_t& p_alignment = 4)
            {
                return new T(); // TODORT implement
            }

            /**
                TODORT docs
            */
            template <typename T> void Free(T* p_pointer)
            {
                delete t; // TODORT implement correctly
            }

        protected:
            uint8_t m_headerSizeInBytes = 2;
            size_t m_numBlocks;
            bool m_shared;
            void* m_currentFree;
        };
    }
}