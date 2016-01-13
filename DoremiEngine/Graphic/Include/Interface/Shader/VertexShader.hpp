#pragma once
#include <string>
struct ID3D11VertexShader;
struct ID3D11InputLayout;
namespace DoremiEngine
{
    namespace Graphic
    {
        class VertexShader
        {
            public:
            virtual const std::string& GetShaderName() = 0;
            virtual ID3D11VertexShader* GetShaderHandle() = 0;
            virtual ID3D11InputLayout* GetInputLayout() = 0;

            virtual void SetShaderName(const std::string& p_shaderName) = 0;
            virtual void SetShaderHandle(ID3D11VertexShader* p_shaderHandle) = 0;
            virtual void SetInputLayout(ID3D11InputLayout* p_inputLayout) = 0;
        };
    }
}