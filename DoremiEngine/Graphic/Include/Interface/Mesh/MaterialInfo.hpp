#pragma once
#include <string>
#include <DoremiEditor/Core/Include/MaterialMessage.hpp>
struct ID3D11ShaderResourceView;
struct ID3D11Buffer;
struct ID3D11SamplerState;
namespace DoremiEngine
{
    namespace Graphic
    {
        class MaterialInfo
        {
        public:
            virtual std::string GetMaterialName() const = 0;
            virtual ID3D11ShaderResourceView* GetTexture() const = 0;
            virtual ID3D11ShaderResourceView* GetGlowTexture() const = 0;
            virtual ID3D11SamplerState* GetSamplerState() const = 0;
            virtual DoremiEditor::Core::MaterialMessage GetMaterialData() const = 0;
            virtual void SetMaterialName(const std::string& p_name) = 0;
            virtual void SetDiffuseTexture(ID3D11ShaderResourceView* p_texture) = 0;
            virtual void SetGlowTexture(ID3D11ShaderResourceView* p_texture) = 0;
            virtual void SetMaterialData(DoremiEditor::Core::MaterialMessage p_data) = 0;
            virtual void SetSamplerState(ID3D11SamplerState* p_samplerState) = 0;
            virtual void SetColor(float r, float g, float b) = 0;

        protected:
            std::string materialName;
            // int mapMasks;
            // float diffuse;
            // float color[3];
            // float ambColor[3];
            // float specColor[3];
            // float specCosine;
            // float specEccentricity;
            // float specRollOff;
            char* diffuseTextureName;
            char* glowTextureName;
            DoremiEditor::Core::MaterialMessage materialData;
        };
    }
}