#pragma once
#include <string>
namespace DoremiEngine
{
    namespace Configuration
    {
        class ConfigurationModule;
    }
    namespace Graphic
    {
        class GraphicModuleImplementation;
        struct GraphicModuleContext
        {
            std::string m_workingDirectory;
            GraphicModuleImplementation* m_graphicModule;
            Configuration::ConfigurationModule& config;
            GraphicModuleContext(Configuration::ConfigurationModule& p_config) : config(p_config) {}
        };
    }
}