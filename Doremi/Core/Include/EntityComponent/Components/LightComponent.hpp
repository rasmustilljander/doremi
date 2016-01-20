#pragma once

#include <DirectXMath.h>

namespace Doremi
{
    namespace Core
    {
        /**
        Contains information that allows entities to emit light. Component can always be active on an
        entity, even when it is not currently emitting light. SwitchState method can be used to change
        state of the light (on/off).*/

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

        struct TransformData
        {
            DirectX::XMFLOAT3 pos;
            DirectX::XMFLOAT4 rot;
            DirectX::XMFLOAT3 scale;
            // Float3 pos, rot, scale;
        };

        struct LightComponent
        {
            // contains information about the light
            LightData lightData;
            // Transform data TODO: Get transform data from Entity and use offset?
            TransformData transform;
            // Use if entity transform data is used
            DirectX::XMFLOAT3 offset;
            // On/off switch
            int enabled;

            void SwitchState() { (enabled == 0) ? 1 : 0; }
        };
    }
}