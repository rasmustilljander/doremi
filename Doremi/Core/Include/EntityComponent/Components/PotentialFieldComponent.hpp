#pragma once
#include <DirectXMath.h>
namespace Doremi
{
    namespace Core
    {
        /**
        Contains information on the charge TODOKO add more needed things
        */
        struct PotentialFieldComponent
        {
            /**
            decides both (-)repulsive and (+)atracting force
            */
            float Power;
            float Area;

            PotentialFieldComponent(const float& p_power, const float& p_area) : Power(p_power), Area(p_area) {}
            PotentialFieldComponent() : Power(0), Area(0) {}
        };
    }
}