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

            /**
            Load model from specified file. Returns modelID
            */
            int LoadObject(const std::string& p_fileName, const std::string& p_materialFileName) override;
            
            /**
            Load shader from specified file. Returns shaderID
            */
            int LoadShader(ShaderType p_shaderType, const std::string& p_fileName) override;

            /**
            Initializes DirectX
            */
            void InitializeDirectX() override;

            /**
            Set specified shader to active
            */
            void BindShader(ShaderType p_shaderType, int p_shaderID) override;

            /**
            Draw objects
            */
            void Draw() override;

            /**
            Computes after effects such as glow and lighting
            */
            void ComputeAfterEffects() override;
        };
    }
}