#pragma once

#include <DoremiEngine/Core/Include/SharedContext.hpp>

#if defined(_WINDLL)
#define ROOTENGINE_DLL_EXPORT __declspec(dllexport)
#else
#define ROOTENGINE_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace Core
    {
        /**
            Interface to engine
        */
        class DoremiEngine
        {
        public:
            /**
                TODO docs
            */
            virtual const SharedContext& GetSharedContext() const = 0;
        };
    }
}

extern "C" {
typedef const DoremiEngine::Core::SharedContext& (*START_ENGINE)(const size_t&);
typedef const void (*STOP_ENGINE)();
ROOTENGINE_DLL_EXPORT const DoremiEngine::Core::SharedContext& StartEngine(const size_t& p_flags);
ROOTENGINE_DLL_EXPORT void StopEngine();
}