#pragma once

#include <Interface/Light/LightInfo.hpp>
#include <DirectXMath.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct Light
        {
            int type; // 0 = def, 1 = dir, 2 = spot, 3 = point
            DirectX::XMFLOAT3 attenuation;
            float intensity;
            DirectX::XMFLOAT3 color;
            float coneAngle;
            DirectX::XMFLOAT3 direction;
            float penumAgle;
            DirectX::XMFLOAT3 position;
        };

        class LightInfoImpl : public LightInfo
        {
            public:
            LightInfoImpl();
            ~LightInfoImpl();
        };
    }
}
