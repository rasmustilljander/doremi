#pragma once
#include <AudioModule.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
    namespace Audio
    {
        class AudioModuleImplementation : public AudioModule
        {
            public:
            /**
                TODO docs
            */
            AudioModuleImplementation(const Core::SharedContext& p_sharedContext);

            /**
                TODO docs
            */
            virtual ~AudioModuleImplementation();

            /**
                TODO docs
            */
            void Startup() override;

            /**
                TODO docs
            */
            void Shutdown() override;

            float DoFunction(float a, float b) override;

            private:
            const Core::SharedContext& m_sharedContext;
        };
    }
}