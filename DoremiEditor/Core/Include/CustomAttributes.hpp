#pragma once
#include <DoremiEditor/Core/Include/SharedVariables.hpp>

namespace DoremiEditor
{
    namespace Core
    {
        struct CustomAttributes
        {
            bool isRendered;
            bool isBBox;
            bool isCollider;
            bool isInteractable;
            float interactIntervalX;
            float interactIntervalY;
            float interactIntervalZ;
            int typeSpawner;
            int typeCheckPoint;
            int typeStartEnd;
            bool isAIground;
        };
    }
}