#pragma once
#include <ConfigurationModule.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
    namespace Configuration
    {
        class ConfigurationModuleImplementation : public ConfigurationModule
        {
        public:
            ConfigurationModuleImplementation(const Core::SharedContext& p_sharedContext);
            virtual ~ConfigurationModuleImplementation();
            void Startup() override;
            void Shutdown() override;

            const ConfiguartionInfo& GetAllConfigurationValues() const override { return m_configInfo; };
            void ReadConfigurationValuesFromFile(const std::string& p_fileName) override;
            void WriteConfigurationValuesToFile(const std::string& p_fileName) override;
            virtual ConfiguartionInfo& GetModifiableConfigurationInfo() override { return m_configInfo; };

        private:
            const Core::SharedContext& m_sharedContext;
            ConfiguartionInfo m_configInfo;
        };
    }
}