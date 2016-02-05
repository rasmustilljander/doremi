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

        private:
            const Core::SharedContext& m_sharedContext;
        };
    }
}