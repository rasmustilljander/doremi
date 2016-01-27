#pragma once
#include <DoremiEditor/Core/Include/SharedVariables.hpp>
#include <DirectXMath.h>

namespace DoremiEditor
{
    namespace Core
    {

        struct MeshData
        {
            int vertCount;
            int normalCount;
            int UVCount;
            int indCount;
            int triCount;
            DirectX::XMFLOAT3* positions;
            DirectX::XMFLOAT3* normals;
            DirectX::XMFLOAT2* uvs;

            int* indexPositions;
            int* indexNormals;
            int* indexUVs;
            int* trianglesPerFace;
            ~MeshData()
            {
                // TODOXX FIX should be deleted/..... Needed for HAX though
                // delete(positions);
                // delete(normals);
                // delete(uvs);

                // delete(indexPositions);
                // delete(indexNormals);
                // delete(indexUVs);
                // delete(trianglesPerFace);
            }
        };
    }
}