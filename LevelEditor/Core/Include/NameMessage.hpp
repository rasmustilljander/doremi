#pragma once
#include <LevelEditor/Core/Include/SharedVariables.hpp>

namespace DoremiEditor
{
    namespace Core
    {

        struct NameMessage // ligger här coz reasons
        {
            char name1[MAX_NAME_SIZE]; // det nya namnet vid namechange
            char name2[MAX_NAME_SIZE];
        };
    }
}