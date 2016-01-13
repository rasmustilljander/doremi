#pragma once
#include <string>
struct ID3D11ComputeShader;
struct ID3D11InputLayout;
namespace DoremiEngine
{
    namespace Graphic
    {
        class ComputeShader
        {
            public:
            virtual const std::string& GetShaderName() = 0;
            virtual ID3D11ComputeShader* GetShaderHandle() = 0;

            virtual void SetShaderName(const std::string& p_shaderName) = 0;
            virtual void SetShaderHandle(ID3D11ComputeShader* p_shaderHandle) = 0;
        };
    }
}