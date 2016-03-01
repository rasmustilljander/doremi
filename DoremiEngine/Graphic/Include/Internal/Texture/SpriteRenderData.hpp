#pragma once
#include <DirectXMath.h>
#include <DoremiEditor/Core/Include/MaterialMessage.hpp>
#include <d3d11_1.h>
#include <Interface/Texture/SpriteInfo.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct SpriteRenderData
        {
            SpriteRenderData() : diffuseTexture(nullptr), samplerState(nullptr) {}
            SpriteRenderData(SpriteData p_spriteData, ID3D11ShaderResourceView* p_texture, ID3D11ShaderResourceView* p_glowtexture,
                             DoremiEditor::Core::MaterialMessage p_materialData, ID3D11SamplerState* p_samplerState)
                : spriteData(p_spriteData), diffuseTexture(p_texture), glowTexture(p_glowtexture), materialMessage(p_materialData), samplerState(p_samplerState)
            {
                /**
                TODO Remove if not needed IE we only used indexdraw
                */
            }

            DoremiEditor::Core::MaterialMessage materialMessage;
            ID3D11ShaderResourceView* diffuseTexture;
            ID3D11ShaderResourceView* glowTexture;
            ID3D11SamplerState* samplerState;
            SpriteData spriteData;
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