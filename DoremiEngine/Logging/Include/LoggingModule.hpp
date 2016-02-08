#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <DoremiEngine/Logging/Include/SubmoduleManager.hpp>
#if defined(_WINDLL)
#define LOGGING_DLL_EXPORT __declspec(dllexport)
#else
#define LOGGING_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace Logging
    {
        class LoggingModule : public DoremiEngine::Core::EngineModule
        {
        public:
            /**

            */
            virtual SubmoduleManager& GetSubModuleManager() const = 0;
        };
    }
}

extern "C" {
typedef DoremiEngine::Logging::LoggingModule* (*CREATE_LOGGING_MODULE)(const DoremiEngine::Core::SharedContext&);
LOGGING_DLL_EXPORT DoremiEngine::Logging::LoggingModule* CreateLoggingModule(const DoremiEngine::Core::SharedContext& p_context);
}