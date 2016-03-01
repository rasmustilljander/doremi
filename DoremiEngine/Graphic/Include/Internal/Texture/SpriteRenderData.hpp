#pragma once
#include <DirectXMath.h>
#include <DoremiEditor/Core/Include/MaterialMessage.hpp>
#include <d3d11_1.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct SpriteRenderData
        {
            SpriteRenderData() : diffuseTexture(nullptr), samplerState(nullptr) {}
            SpriteRenderData(ID3D11Buffer* p_spriteInfoCBuffer, ID3D11ShaderResourceView* p_texture, ID3D11ShaderResourceView* p_glowtexture,
                             DoremiEditor::Core::MaterialMessage p_materialData, ID3D11SamplerState* p_samplerState)
                : spriteInfoCBuffer(p_spriteInfoCBuffer), diffuseTexture(p_texture), glowTexture(p_glowtexture), materialMessage(p_materialData), samplerState(p_samplerState)
            {
                /**
                TODO Remove if not needed IE we only used indexdraw
                */
            }

            DoremiEditor::Core::MaterialMessage materialMessage;
            ID3D11ShaderResourceView* diffuseTexture;
            ID3D11ShaderResourceView* glowTexture;
            ID3D11SamplerState* samplerState;
            ID3D11Buffer* spriteInfoCBuffer;
        };

        inline bool SortOnTextureThenVertex(SpriteRenderData& a, MeshRenderData& b)
        {
            if(a.diffuseTexture < b.diffuseTexture)
            {
                return true;
            }
            return false;
        }
    }
}