#pragma once
#include <AudioModule.hpp>

namespace DoremiEngine
{
    namespace Audio
    {
        class AudioModuleImplementation : public AudioModule
        {
            public:
            /**
                TODO docs
            */
            AudioModuleImplementation();

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
        };
    }
}