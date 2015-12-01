#pragma once
#include <InputModule.hpp>

namespace DoremiEngine
{
    namespace Input
    {
        class InputModuleImplementation : public InputModule
        {
            public:
            /**
                TODO docs
            */
            InputModuleImplementation();

            /**
                TODO docs
            */
            virtual ~InputModuleImplementation();

            /**
                TODO docs
            */
            void Startup() override;

            /**
                TODO docs
            */
            void SetWorkingDirectory(const std::string& p_workingDirectory) override;

            /**
                TODO docs
            */
            void Shutdown() override;

            float DoFunction(float a, float b) override;
        };
    }
}