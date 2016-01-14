#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct MeshRenderData
        {
            MeshRenderData() : texture(nullptr), samplerState(nullptr), vertexData(nullptr), indexData(nullptr) {}
            MeshRenderData(DirectX::XMFLOAT4X4 p_worldMatrix, ID3D11ShaderResourceView* p_texture, ID3D11SamplerState* p_samplerState,
                           ID3D11Buffer* p_vertexData, const size_t& p_vertexCount)
                : worldMatrix(p_worldMatrix),
                  texture(p_texture),
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
            MeshRenderData(DirectX::XMFLOAT4X4 p_worldMatrix, ID3D11ShaderResourceView* p_texture, ID3D11SamplerState* p_samplerState,
                           ID3D11Buffer* p_vertexData, const size_t& p_vertexCount, ID3D11Buffer* p_indexData, const size_t& p_indexCount)
                : worldMatrix(p_worldMatrix),
                  texture(p_texture),
                  samplerState(p_samplerState),
                  vertexData(p_vertexData),
                  vertexCount(p_vertexCount),
                  indexData(p_indexData),
                  indexCount(p_indexCount)
            {
            }
            ID3D11Buffer* indexData;
            ID3D11Buffer* vertexData;
            ID3D11ShaderResourceView* texture;
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
            if(a.texture < b.texture)
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