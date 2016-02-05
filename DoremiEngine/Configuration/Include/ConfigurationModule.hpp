#pragma once
#include <DoremiEngine/Core/Include/Subsystem/EngineModule.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

#if defined(_WINDLL)
#define CONFIGURATION_DLL_EXPORT __declspec(dllexport)
#else
#define CONFIGURATION_DLL_EXPORT __declspec(dllimport)
#endif

namespace DoremiEngine
{
    namespace Configuration
    {
        /**
        TODO docs
        */
        class ConfigurationModule : public DoremiEngine::Core::EngineModule
        {
        public:
        };
    }
}

extern "C" {
typedef DoremiEngine::Configuration::ConfigurationModule* (*CREATE_CONFIGURATION_MODULE)(const DoremiEngine::Core::SharedContext&);
CONFIGURATION_DLL_EXPORT DoremiEngine::Configuration::ConfigurationModule* CreateConfigurationModule(const DoremiEngine::Core::SharedContext& p_context);
}