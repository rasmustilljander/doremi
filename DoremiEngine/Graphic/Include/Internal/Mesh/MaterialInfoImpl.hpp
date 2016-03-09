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
            ID3D11ShaderResourceView* GetGlowTexture() const override { return m_glowTexture; };
            ID3D11SamplerState* GetSamplerState() const override { return m_samplerState; }
            DoremiEditor::Core::MaterialMessage GetMaterialData() const override { return materialData; }
            void SetMaterialName(const std::string& p_name) override { m_name = std::string(p_name); };
            void SetDiffuseTexture(ID3D11ShaderResourceView* p_texture) override { m_texture = p_texture; };
            void SetGlowTexture(ID3D11ShaderResourceView* p_texture) override { m_glowTexture = p_texture; };
            void SetMaterialData(DoremiEditor::Core::MaterialMessage p_data) override { materialData = p_data; };
            void SetSamplerState(ID3D11SamplerState* p_samplerState) override { m_samplerState = p_samplerState; }
            void SetColor(float r, float g, float b) override { materialData.SetColor(r, g, b); }
            void SetAlpha(float a) override { materialData.SetAlpha(a); }

        private:
            std::string m_name;
            ID3D11ShaderResourceView* m_texture;
            ID3D11ShaderResourceView* m_glowTexture;
            ID3D11SamplerState* m_samplerState;
        };
    }
}