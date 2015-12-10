#include <MemoryManager.hpp>
#include <Allocator/Raw/RawAllocator.hpp>

namespace Utility
{
    namespace Memory
    {

        MemoryManager* MemoryManager::m_instance = nullptr;

        void MemoryManager::Startup(const size_t& p_preferedSize)
        {
            if(m_instance != nullptr)
            {
                m_instance = new MemoryManager(p_preferedSize);
            }
            // TODORT logging
        }

        void MemoryManager::Shutdown()
        {
            delete m_instance;
            m_instance = nullptr;
        }

        MemoryManager& MemoryManager::GetInstance() { return *m_instance; }

        MemoryManager::MemoryManager(const size_t& p_preferedSize)
        {
            m_rawAllocator = std::unique_ptr<RawAllocator>(new RawAllocator());
            m_rawAllocator.get()->Initialize(p_preferedSize);
        }

        MemoryManager::~MemoryManager() {}
    }
}