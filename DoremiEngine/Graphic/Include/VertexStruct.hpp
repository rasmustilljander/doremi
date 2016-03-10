#pragma once
#include <DirectXMath.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        struct VertexBasic // TODOKO Remove!
        {
            float x, y, z;
            float texcoord[2];
            float nx, ny, nz;
        };
        struct Vertex // TODOKO one with the same name exists in Mesh/Vertex, outcommented for now
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT2 textureCoordinate;
            DirectX::XMFLOAT3 normal;
        };

        struct SkeletalVertex
        {
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT2 textureCoordinate;
            DirectX::XMFLOAT3 normal;
            int jointID;
        };
        struct VertexPos;
    }
}