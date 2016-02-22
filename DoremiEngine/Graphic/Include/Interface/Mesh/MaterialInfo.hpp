#pragma once
#include <string>
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
            virtual ID3D11Buffer* GetMaterialData() const = 0;
            virtual void SetMaterialName(const std::string& p_name) = 0;
            virtual void SetDiffuseTexture(ID3D11ShaderResourceView* p_texture) = 0;
            virtual void SetGlowTexture(ID3D11ShaderResourceView* p_texture) = 0;
            virtual void SetMaterialData(ID3D11Buffer* p_data) = 0;
            virtual void SetSamplerState(ID3D11SamplerState* p_samplerState) = 0;

        private:
            int mapMasks;
            float diffuse;
            float color[3];
            float ambColor[3];
            float specColor[3];
            float specCosine;
            float specEccentricity;
            float specRollOff;
            char* diffuseTextureName;
            char* glowTextureName;
            ID3D11Buffer* materialData;
        };
    }
}