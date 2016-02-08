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
        A struct containing all the information gathered from config files TODOKO make one for each module
        */
        struct ConfiguartionInfo
        {
            // Graphic stuff
            float ScreenWidth = 800;
            float ScreenHeight = 600;

            // Audio stuff
            float Volume = 0.5f;

            // Key binds TODOKO ask how this works
        };
        /**
        Reads and saves configuration from file. If another module needs configuration values they can use fucntions in this class to get them.
        */
        class ConfigurationModule : public DoremiEngine::Core::EngineModule
        {
        public:
            /**
            Returns a non modifiable struct with all configuration values
            */
            const ConfiguartionInfo& GetAllConfigurationValues();
        };
    }
}

extern "C" {
typedef DoremiEngine::Configuration::ConfigurationModule* (*CREATE_CONFIGURATION_MODULE)(const DoremiEngine::Core::SharedContext&);
CONFIGURATION_DLL_EXPORT DoremiEngine::Configuration::ConfigurationModule* CreateConfigurationModule(const DoremiEngine::Core::SharedContext& p_context);
}