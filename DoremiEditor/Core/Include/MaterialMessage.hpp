#pragma once
#include <DoremiEditor/Core/Include/SharedVariables.hpp>
#include <DoremiEditor/Core/Include/MaterialData.hpp>

namespace DoremiEditor
{
    namespace Core
    {
        struct MaterialMessage
        {
            char nodeName[MAX_NAME_SIZE];
            char diffuseTexturePath[MAX_NAME_SIZE];
            char glowTexturePath[MAX_NAME_SIZE];
            char specTexturePath[MAX_NAME_SIZE];
            char bumpTexturePath[MAX_NAME_SIZE];
            int type;
            MaterialData data;
        };
    }
}