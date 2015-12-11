#include <MainMemoryManager.hpp>
#include <Allocator/Raw/RawAllocator.hpp>

namespace Utility
{
    namespace MemoryManager
    {

        MainMemoryManager* MainMemoryManager::m_instance = nullptr;

        void MainMemoryManager::Startup(const size_t& p_preferedSize)
        {
            if(m_instance == nullptr)
            {
                m_instance = new MainMemoryManager(p_preferedSize);
            }
            // TODORT logging
        }

        void MainMemoryManager::Shutdown()
        {
            delete m_instance;
            m_instance = nullptr;
        }

        MainMemoryManager& MainMemoryManager::GetInstance() { return *m_instance; }

        MainMemoryManager::MainMemoryManager(const size_t& p_preferedSize)
        {
            m_applicationAllocator = new RawAllocator();
            static_cast<RawAllocator*>(m_applicationAllocator)->Initialize(p_preferedSize, 4, true); // TODORT hardcoded
        }

        MainMemoryManager::~MainMemoryManager() { delete m_applicationAllocator; }
    }
}