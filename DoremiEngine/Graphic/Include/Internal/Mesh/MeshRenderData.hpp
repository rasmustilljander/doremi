#pragma once
#include <DirectXMath.h>
#include <DoremiEditor/Core/Include/MaterialMessage.hpp>
#include <d3d11_1.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct MeshRenderData
        {
            MeshRenderData() : diffuseTexture(nullptr), samplerState(nullptr), vertexData(nullptr), indexData(nullptr) {}
            MeshRenderData(DirectX::XMFLOAT4X4 p_worldMatrix, ID3D11ShaderResourceView* p_texture, ID3D11ShaderResourceView* p_glowtexture,
                           DoremiEditor::Core::MaterialMessage p_materialData, ID3D11SamplerState* p_samplerState, ID3D11Buffer* p_vertexData,
                           const size_t& p_vertexCount)
                : worldMatrix(p_worldMatrix),
                  diffuseTexture(p_texture),
                  glowTexture(p_glowtexture),
                  materialMessage(p_materialData),
                  samplerState(p_samplerState),
                  vertexData(p_vertexData),
                  vertexCount(p_vertexCount),
                  indexData(nullptr),
                  indexCount(0)
            {
                /**
                TODO Remove if not needed IE we only used indexdraw
                */
            }
            MeshRenderData(DirectX::XMFLOAT4X4 p_worldMatrix, ID3D11ShaderResourceView* p_texture, ID3D11ShaderResourceView* p_glowtexture,
                           ID3D11SamplerState* p_samplerState, ID3D11Buffer* p_vertexData, const size_t& p_vertexCount, ID3D11Buffer* p_indexData,
                           const size_t& p_indexCount, DoremiEditor::Core::MaterialMessage p_materialData)
                : worldMatrix(p_worldMatrix),
                  diffuseTexture(p_texture),
                  glowTexture(p_glowtexture),
                  samplerState(p_samplerState),
                  vertexData(p_vertexData),
                  vertexCount(p_vertexCount),
                  indexData(p_indexData),
                  indexCount(p_indexCount),
                  materialMessage(p_materialData)
            {
            }
            ID3D11Buffer* indexData;
            ID3D11Buffer* vertexData;
            DoremiEditor::Core::MaterialMessage materialMessage;
            ID3D11ShaderResourceView* diffuseTexture;
            ID3D11ShaderResourceView* glowTexture;
            ID3D11SamplerState* samplerState;
            DirectX::XMFLOAT4X4 worldMatrix;
            size_t vertexCount;
            size_t indexCount;
        };

        inline bool SortOnVertexThenTexture(MeshRenderData& a, MeshRenderData& b)
        {
            if(a.vertexData < b.vertexData)
            {
                return true;
            }
            // if(a.texture < b.texture)
            //{
            //    return true;
            //}
            return false;
        }

        inline bool SortOnTextureThenVertex(MeshRenderData& a, MeshRenderData& b)
        {
            if(a.diffuseTexture < b.diffuseTexture)
            {
                return true;
            }
            // if(a.vertexData < b.vertexData)
            //{
            //    return true;
            //}
            return false;
        }
    }
}