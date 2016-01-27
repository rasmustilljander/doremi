#pragma once
#include <LevelEditor/Core/Include/SharedVariables.hpp>

namespace DoremiEditor
{
    namespace Core
    {

        struct CameraData
        {
            int isOrtho;
            float target[3];
            float upVector[3];
            float rightVector[3];
            float hAngle; // hor-FOV
        };
    }
}