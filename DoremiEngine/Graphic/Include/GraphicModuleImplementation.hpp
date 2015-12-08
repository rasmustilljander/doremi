#pragma once
#include <GraphicModule.hpp>


namespace DoremiEngine
{
    namespace Graphic
    {
        class GraphicMain;
        class GraphicModuleImplementation : public GraphicModule
        {
            public:
            /**
            TODO docs
            */
            GraphicModuleImplementation(const Core::SharedContext& p_sharedContext);

            /**
            TODO docs
            */
            virtual ~GraphicModuleImplementation();

            /**
            TODORT docs
            */
            void Update() override;

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
            void Draw(const int& p_meshID, const int& p_textureID, const DirectX::XMFLOAT4X4& p_translationMatrix) override;

            /**
            Computes after effects such as glow and lighting
            */
            void ComputeAfterEffects() override;

            /**
            Swaps buffers and clears backbuffer
            */
            void EndDraw() override;

        private:
            const Core::SharedContext& m_sharedContext;
            GraphicMain* m_graphicMain;
        };
    }
}