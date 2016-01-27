#pragma once
#include <LevelEditor/Core/Include/SharedVariables.hpp>
#include <LevelEditor/Core/Include/TransformData.hpp>

namespace DoremiEditor
{
    namespace Core
    {
        struct TransformMessage
        {
            char objectName[MAX_NAME_SIZE]; // ifall dessa ska ändras till dynamiskt allokerade så kolla FileHandler->CorrectName!!
            char parentName[MAX_NAME_SIZE];
            TransformData transformData;
        };
    }
}