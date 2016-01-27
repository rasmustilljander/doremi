#pragma once
#include <DoremiEditor/Core/Include/SharedVariables.hpp>
#include <DoremiEditor/Core/Include/TransformData.hpp>

namespace DoremiEditor
{
    namespace Core
    {
        struct TransformMessage
        {
            char nodeName[MAX_NAME_SIZE]; // ifall dessa ska ändras till dynamiskt allokerade så kolla FileHandler->CorrectName!!
            char parentName[MAX_NAME_SIZE];
            TransformData transformData;
        };
    }
}