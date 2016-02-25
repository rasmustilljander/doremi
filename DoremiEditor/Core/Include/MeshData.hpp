#pragma once
#include <DoremiEditor/Core/Include/SharedVariables.hpp>
#include <DirectXMath.h>

namespace DoremiEditor
{
    namespace Core
    {

        struct MeshData
        {
            MeshData()
                : positions(nullptr), normals(nullptr), uvs(nullptr), indexPositions(nullptr), indexNormals(nullptr), indexUVs(nullptr), trianglesPerFace(nullptr)
            {
            }

            ~MeshData()
            {
                delete positions;
                delete normals;
                delete uvs;
                delete indexPositions;
                delete indexNormals;
                delete indexUVs;
                delete trianglesPerFace;
            }

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
        };
    }
}