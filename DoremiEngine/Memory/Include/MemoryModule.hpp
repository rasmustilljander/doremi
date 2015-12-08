#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

#if defined(_WINDLL)
#define MEMORY_DLL_EXPORT __declspec(dllexport)
#else
#define MEMORY_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace Memory
    {
        /**
        TODO docs
        */
        class MemoryModule : public DoremiEngine::Core::EngineModule
        {
            public:
        };
    }
}

extern "C" {
typedef DoremiEngine::Memory::MemoryModule* (*CREATE_MEMORY_MODULE)(const DoremiEngine::Core::SharedContext&);
MEMORY_DLL_EXPORT DoremiEngine::Memory::MemoryModule* CreateMemoryModule(const DoremiEngine::Core::SharedContext& p_context);
}