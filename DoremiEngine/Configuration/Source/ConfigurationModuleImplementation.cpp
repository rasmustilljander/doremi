#include <Internal/ConfigurationModuleImplementation.hpp>
namespace DoremiEngine
{
    namespace Configuration
    {
        ConfigurationModuleImplementation::ConfigurationModuleImplementation(const Core::SharedContext& p_sharedContext)
            : m_sharedContext(p_sharedContext)
        {
        }

        ConfigurationModuleImplementation::~ConfigurationModuleImplementation() {}

        void ConfigurationModuleImplementation::Startup() {}

        void ConfigurationModuleImplementation::Shutdown() {}
    }
}

DoremiEngine::Configuration::ConfigurationModule* CreateConfigurationModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Configuration::ConfigurationModule* configuration = new DoremiEngine::Configuration::ConfigurationModuleImplementation(p_sharedContext);
    return configuration;
}