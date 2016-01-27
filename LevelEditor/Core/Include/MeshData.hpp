#pragma once
#include <LevelEditor/Core/Include/SharedVariables.hpp>
#include <DirectXMath.h>

namespace DoremiEditor
{
    namespace Core
    {

        struct MeshData
        {
            int nrPos, nrNor, nrUV, nrI;
            int triangleCount;
            DirectX::XMFLOAT3* positions;
            DirectX::XMFLOAT3* normals;
            DirectX::XMFLOAT2* uvs;

            int* indexPositions;
            int* indexNormals;
            int* indexUVs;
            // int *indexTriangles;
            int* trianglesPerFace;
            // Material *material; //pekar på ett specifikt material i en vektor av material
            ~MeshData()
            {
                delete(positions);
                delete(normals);
                delete(uvs);

                delete(indexPositions);
                delete(indexNormals);
                delete(indexUVs);
                delete(trianglesPerFace);
            }
        };
    }
}