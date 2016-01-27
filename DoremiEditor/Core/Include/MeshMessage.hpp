#pragma once
#include <DoremiEditor/Core/Include/SharedVariables.hpp>
#include <DoremiEditor/Core/Include/MeshData.hpp>

namespace DoremiEditor
{
    namespace Core
    {
        struct MeshMessage
        {
            char objectName[MAX_NAME_SIZE];
            char transformName[MAX_NAME_SIZE];
            char materialName[MAX_NAME_SIZE];
            int meshID; // kolla ifall denna meshen ska instance draw:as!
            int materialID;
            MeshData meshData;
        };
    }
}