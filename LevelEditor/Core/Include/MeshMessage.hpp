#pragma once
#include <LevelEditor/Core/Include/SharedVariables.hpp>

namespace DoremiEditor
{
    namespace Core
    {
        struct MeshData;

        struct MeshMessage
        {
            char objectName[MAX_NAME_SIZE];
            char transformName[MAX_NAME_SIZE];
            char materialName[MAX_NAME_SIZE];
            int meshID; // kolla ifall denna meshen ska instance draw:as!
            int materialID;
            MeshData* meshData;
        };
    }
}