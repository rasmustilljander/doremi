#pragma once
namespace DoremiEngine
{
    namespace Configuration
    {
        class ConfigurationModule;
    }
    namespace AI
    {
        class AIModuleImplementation;
        class AStarSubModuleImpl;
        class PotentialFieldSubModuleImpl;

        struct AIContext
        {
            AIModuleImplementation* Module;
            AStarSubModuleImpl* AStarModule;
            PotentialFieldSubModuleImpl* PFModule;
            Configuration::ConfigurationModule& config;
            AIContext(Configuration::ConfigurationModule& p_config) : config(p_config){};
        };
    }
}