#pragma once
#include <Interface/Shader/PixelShader.hpp>
namespace DoremiEngine
{
    namespace Graphic
    {
        class PixelShaderImpl : public PixelShader
        {
            public:
            PixelShaderImpl();
            virtual ~PixelShaderImpl();
            const std::string& GetShaderName() override;
            ID3D11PixelShader* GetShaderHandle() override;

            virtual void SetShaderName(const std::string& p_shaderName) override { m_shaderName = p_shaderName; };
            virtual void SetShaderHandle(ID3D11PixelShader* p_shaderHandle) override { m_shaderHandle = p_shaderHandle; };

            private:
            ID3D11PixelShader* m_shaderHandle;
            std::string m_shaderName;
        };
    }
}