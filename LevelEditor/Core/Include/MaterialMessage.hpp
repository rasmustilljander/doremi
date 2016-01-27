#pragma once
#include <LevelEditor/Core/Include/SharedVariables.hpp>
#include <LevelEditor/Core/Include/MaterialData.hpp>

namespace DoremiEditor
{
    namespace Core
    {
        struct MaterialMessage
        {
            char objectName[MAX_NAME_SIZE];
            char textureName[MAX_NAME_SIZE];
            int materialType;
            MaterialData materialData;
        };
    }
}