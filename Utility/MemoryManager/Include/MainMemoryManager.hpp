#pragma once
#include <Utility/MemoryManager/Include/Allocator/MemoryAllocator.hpp>

namespace Utility
{
    namespace MemoryManager
    {
        /**
        TODORT docs
        */
        class MainMemoryManager
        {
            public:
            /**
            TODORT docs
            */
            static void Startup(const size_t& p_preferedSize = 512'000'000);

            /**
            TODORT docs
            */
            static void Shutdown();
            /**
            TODORT docs
            */
            static MainMemoryManager& GetInstance();

            /**
            TODORT docs
            */
            MainMemoryManager(MainMemoryManager const&) = delete;

            /**
            TODORT docs
            */
            void operator=(MainMemoryManager const&) = delete;

            private:
            // Non-static
            MainMemoryManager(const size_t& p_preferedSize);
            virtual ~MainMemoryManager();
            MemoryAllocator* m_applicationAllocator;

            // Static
            static MainMemoryManager* m_instance;
        };
    }
}