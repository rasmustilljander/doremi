#pragma once
#include <Interface/Shader/VertexShader.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        class VertexShaderImpl : public VertexShader
        {
        public:
            VertexShaderImpl();
            virtual ~VertexShaderImpl();
            const std::string& GetShaderName() override;
            ID3D11VertexShader* GetShaderHandle() override;
            ID3D11InputLayout* GetInputLayout() override;

            virtual void SetShaderName(const std::string& p_shaderName) override { m_shaderName = p_shaderName; };
            void SetShaderHandle(ID3D11VertexShader* p_shaderHandle)override { m_shaderHandle = p_shaderHandle; };
            void SetInputLayout(ID3D11InputLayout* p_inputLayout)override { m_inputLayout = p_inputLayout; };
        private:
            std::string m_shaderName;
            ID3D11VertexShader* m_shaderHandle;
            ID3D11InputLayout* m_inputLayout;
        };
    }
}