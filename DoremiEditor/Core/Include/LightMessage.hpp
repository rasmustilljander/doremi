#pragma once
#include <DoremiEditor/Core/Include/SharedVariables.hpp>
#include <DoremiEditor/Core/Include/LightData.hpp>

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