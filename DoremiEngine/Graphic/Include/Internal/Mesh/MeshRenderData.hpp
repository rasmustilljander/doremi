#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct MeshRenderData
        {
            MeshRenderData() {}
            MeshRenderData(DirectX::XMFLOAT4X4 p_worldMatrix, ID3D11ShaderResourceView* p_texture, ID3D11Buffer* p_vertexData, const size_t& p_vertexCount)
                : worldMatrix(p_worldMatrix), texture(p_texture), vertexData(p_vertexData), vertexCount(p_vertexCount)
            {
            }
            ID3D11ShaderResourceView* texture;
            ID3D11Buffer* vertexData;
            DirectX::XMFLOAT4X4 worldMatrix;
            size_t vertexCount;
        };

        inline bool SortOnVertexThenTexture(MeshRenderData& a, MeshRenderData& b)
        {
            if(a.vertexData < b.vertexData)
            {
                return true;
            }
            if(a.texture < b.texture)
            {
                return true;
            }
            return false;
        }

        inline bool SortOnTextureThenVertex(MeshRenderData& a, MeshRenderData& b)
        {
            if(a.texture < b.texture)
            {
                return true;
            }
            if(a.vertexData < b.vertexData)
            {
                return true;
            }
            return false;
        }
    }
}