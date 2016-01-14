#pragma once
#include <Interface/Shader/ComputeShader.hpp>
#include <DirectXMath.h>
namespace DoremiEngine
{
    namespace Graphic
    {

        struct ScreenToViewParams
        {
            DirectX::XMFLOAT4X4 inverseProjection;
            DirectX::XMFLOAT2 screenSize;
            DirectX::XMFLOAT2 pad;
        };

        class ComputeShaderImpl : public ComputeShader
        {
        public:
            ComputeShaderImpl();
            virtual ~ComputeShaderImpl();
            const std::string& GetShaderName() override;
            ID3D11ComputeShader* GetShaderHandle() override;

            virtual void SetShaderName(const std::string& p_shaderName) override { m_shaderName = p_shaderName; };
            virtual void SetShaderHandle(ID3D11ComputeShader* p_shaderHandle) override { m_shaderHandle = p_shaderHandle; };

        private:
            ID3D11ComputeShader* m_shaderHandle;
            std::string m_shaderName;
        };
    }
}
