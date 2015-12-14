#pragma once

#include <vector>

namespace DoremiEngine
{
    namespace Graphic
    {
        class LightInfo;
        class LightManager
        {
            // TODORK revise structure
            public:
            virtual int AddLight(LightInfo light) = 0;
            virtual LightInfo GetLight(int index) = 0;
            virtual std::vector<LightInfo> GetLightList() = 0;
        };
    }
}