#pragma once

#include <vector>
#include <DirectXMath.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        class LightInfoImpl;
        struct Light;
        class LightManager
        {
            // TODORK revise structure
        public:
            virtual Light* AddLight(int type, float intensity, DirectX::XMFLOAT3 color, float coneAngle, DirectX::XMFLOAT3 direction,
                                    float penumAngle, DirectX::XMFLOAT3 position) = 0;
            virtual void InitLightManager() = 0;
            virtual void TestFunc() = 0;
        };
    }
}