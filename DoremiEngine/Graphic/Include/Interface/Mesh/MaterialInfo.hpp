#pragma once
#include <string>
struct ID3D11ShaderResourceView;
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
            virtual ID3D11SamplerState* GetSamplerState() const = 0;
            virtual void SetMaterialName(const std::string& p_name) = 0;
            virtual void SetTexture(ID3D11ShaderResourceView* p_texture) = 0;
            virtual void SetSamplerState(ID3D11SamplerState* p_samplerState) = 0;
        };
    }
}