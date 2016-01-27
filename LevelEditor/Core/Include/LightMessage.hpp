#pragma once
#include <LevelEditor/Core/Include/SharedVariables.hpp>
#include <LevelEditor/Core/Include/LightData.hpp>

namespace DoremiEditor
{
    namespace Core
    {
        struct LightMessage
        {
            char objectName[MAX_NAME_SIZE];
            char transformName[MAX_NAME_SIZE];
            LightData lightdata;
        };
    }
}