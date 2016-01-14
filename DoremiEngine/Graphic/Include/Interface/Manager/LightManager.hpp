#pragma once

#include <vector>

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
            virtual int AddLight(Light light) = 0;
            virtual Light GetLight(int index) = 0;
            virtual void InitLightManager() = 0;
        };
    }
}