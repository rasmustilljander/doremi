#pragma once
#include <Interface/Mesh/MaterialInfo.hpp>
namespace DoremiEngine
{
    namespace Graphic
    {
        class MaterialInfoImpl : public MaterialInfo
        {
        public:
            MaterialInfoImpl();
            virtual ~MaterialInfoImpl();
            std::string GetMaterialName() const override { return m_name; };
            ID3D11ShaderResourceView* GetTexture() const override { return m_texture; };
            ID3D11SamplerState* GetSamplerState() const override { return m_samplerState; }
            void SetMaterialName(const std::string& p_name) override { m_name = p_name; };
            void SetTexture(ID3D11ShaderResourceView* p_texture) override { m_texture = p_texture; };
            void SetSamplerState(ID3D11SamplerState* p_samplerState) override { m_samplerState = p_samplerState; }
        private:
            std::string m_name;
            ID3D11ShaderResourceView* m_texture;
            ID3D11SamplerState* m_samplerState;
        };
    }
}