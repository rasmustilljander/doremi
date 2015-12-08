#include <Internal/MemoryModuleImpl.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

namespace DoremiEngine
{
    namespace Memory
    {
        MemoryModuleImplementation::MemoryModuleImplementation(const Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}

        MemoryModuleImplementation::~MemoryModuleImplementation() {}

        void MemoryModuleImplementation::Startup() {}

        void MemoryModuleImplementation::Shutdown() {}
    }
}

DoremiEngine::Memory::MemoryModule* CreateMemoryModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Memory::MemoryModule* memory = new DoremiEngine::Memory::MemoryModuleImplementation(p_sharedContext);
    return memory;
}