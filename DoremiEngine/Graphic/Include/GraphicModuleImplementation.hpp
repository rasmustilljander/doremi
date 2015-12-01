#pragma once
#include <GraphicModule.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        class GraphicModuleImplementation : public GraphicModule
        {
            public:
            /**
            TODO docs
            */
            GraphicModuleImplementation();

            /**
            TODO docs
            */
            virtual ~GraphicModuleImplementation();

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

        };
    }
}