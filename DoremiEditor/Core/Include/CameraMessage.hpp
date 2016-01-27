#pragma once
#include <DoremiEditor/Core/Include/SharedVariables.hpp>
#include <DoremiEditor/Core/Include/CameraData.hpp>

namespace DoremiEditor
{
    namespace Core
    {
        struct CameraMessage
        {
            char nodeName[MAX_NAME_SIZE];
            char transformName[MAX_NAME_SIZE];
            CameraData cameraData;
        };
    }
}