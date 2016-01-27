#pragma once
#include <LevelEditor/Core/Include/SharedVariables.hpp>
#include <LevelEditor/Core/Include/CameraData.hpp>

namespace DoremiEditor
{
    namespace Core
    {
        struct CameraMessage
        {
            char objectName[MAX_NAME_SIZE];
            char transformName[MAX_NAME_SIZE];
            CameraData cameraData;
        };
    }
}