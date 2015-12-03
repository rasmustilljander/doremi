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
            //GraphicModuleImplementation();

            GraphicModuleImplementation(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {};
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

            /**
            Swaps buffers and clears backbuffer
            */
            void EndDraw() override;

        private:
            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}