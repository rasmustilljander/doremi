#pragma once
#include <memory>
#include <Utility/MemoryManager/Include/Allocator/Raw/RawAllocator.hpp>

namespace Utility
{
    namespace Memory
    {

        /**
        TODORT docs
        */
        class MemoryManager
        {
            public:
            static void Startup(const size_t& p_preferedSize = 510'000'000);
            static void Shutdown();
            static MemoryManager& GetInstance();

            template <typename T> T* AllocateInGlobal() { return m_rawAllocator.get()->Allocate<T>(); }

            //  template<typename T>
            //  StackAllocator<T>& BuildStackAllocator();

            //   template<typename T>
            //    ComponentAllocator<T>& BuildComponentAllocator(const size_t& p_preferedSize = 1000*sizeof(T)); // TODORT fix predefined size

            MemoryManager(MemoryManager const&) = delete;
            void operator=(MemoryManager const&) = delete;

            private:
            // Non-static
            MemoryManager(const size_t& p_preferedSize);
            virtual ~MemoryManager();
            std::unique_ptr<RawAllocator> m_rawAllocator;

            // Static
            static MemoryManager* m_instance;
        };
    }
}