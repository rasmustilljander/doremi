#pragma once

#include <Interface/Light/LightInfo.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct Light
        {
            int type; // 0 = def, 1 = dir, 2 = spot, 3 = point
            float constant, linear, quadratic;
            float intensity;
            float r, g, b;
            float dirX, dirY, dirZ;
            float posX, posY, posZ;
            float coneAngle;
            float penumAgle;
        };

        class LightInfoImpl : public LightInfo
        {
            public:
            LightInfoImpl();
            ~LightInfoImpl();
        };
    }
}
