#pragma once
#include <Interface/Manager/ShaderManager.hpp>
#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {
        struct GraphicModuleContext;

        class ShaderManagerImpl : public ShaderManager
        {
        public:
            // TODOKO Add docs
            ShaderManagerImpl(const GraphicModuleContext& p_graphicContext);
            virtual ~ShaderManagerImpl();
            VertexShader* BuildVertexShader(const std::string& p_fileName, D3D11_INPUT_ELEMENT_DESC p_inputDescription[], int p_arraySize) override;
            VertexShader * BuildVertexShaderWithoutInput(const std::string & p_fileName) override;
            PixelShader* BuildPixelShader(const std::string& p_fileName) override;
            GeometryShader* BuildGeometryShader(const std::string& p_fileName);
            ComputeShader* BuildComputeShader(const std::string& p_fileName) override;

            void SetActiveVertexShader(VertexShader* p_shader) override;
            void SetActivePixelShader(PixelShader* p_shader) override;
            void SetActiveGeometryShader(GeometryShader* p_shader) override;
            void RemoveGeometryShader() override;
            void SetActiveComputeShader(ComputeShader* p_shader) override;

        private:
            const GraphicModuleContext& m_graphicContext;
        };
    }
}