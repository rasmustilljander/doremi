#pragma once
#include <algorithm>
namespace Doremi
{
    namespace Core
    {
        /**
        This namespace contains equations for calculating the force impact of a actor in a field. Only usable with potential vs others
        Note: All fucntions must take 3 floats as in param and return a float
        */
        namespace ForceEquations
        {
            static float Standard(float p_actorCharge, float p_distance, float p_actorRange)
            {
                return p_actorCharge * std::fmaxf(1.0f - p_distance / p_actorRange, 0.0f);
            }
        }
    }
}