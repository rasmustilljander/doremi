#pragma once
#include <string>
class ID3D11PixelShader;
namespace DoremiEngine
{
    namespace Graphic
    {
        class PixelShader
        {
            public:
            virtual const std::string& GetShaderName() = 0;
            virtual ID3D11PixelShader* GetShaderHandle() = 0;

            virtual void SetShaderName(const std::string& p_shaderName) = 0;
            virtual void SetShaderHandle(ID3D11PixelShader* p_shaderHandle) = 0;
        };
    }
}