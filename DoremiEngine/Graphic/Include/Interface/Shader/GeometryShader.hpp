#pragma once
#include <string>
struct ID3D11GeometryShader;

namespace DoremiEngine
{
    namespace Graphic
    {
        class GeometryShader
        {
        public:
            virtual const std::string& GetShaderName() = 0;
            virtual ID3D11GeometryShader* GetShaderHandle() = 0;

            virtual void SetShaderName(const std::string& p_shaderName) = 0;
            virtual void SetShaderHandle(ID3D11GeometryShader* p_shaderHandle) = 0;
        };
    }
}