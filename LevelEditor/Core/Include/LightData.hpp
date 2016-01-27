#pragma once
#include <LevelEditor/Core/Include/SharedVariables.hpp>

namespace DoremiEditor
{
    namespace Core
    {
        struct LightData
        {
            int type; // 0 = def, 1 = dir, 2 = spot, 3 = point
            int decayType; // 0 = none, 1 = linear, 2 = quadratic (l/d**v)
            float intensity;
            DirectX::XMFLOAT3 colorDiffuse;
            DirectX::XMFLOAT3 direction;
            float dropOff;
            float coneAngle;
            float penumAgle;
        };
    }
}