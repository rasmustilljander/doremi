#pragma once
#include <DirectXMath.h>
namespace DoremiEngine
{
    namespace Physics
    {
        /**
        Contains functions for raycasting TODOKO add more raycasting related stuff
        */
        class RayCastManager
        {
        public:
            /**
            Returns the id of the body hit by the ray. Returns -1 if no hit occured
            */
            virtual int CastRay(const DirectX::XMFLOAT3& p_origin, const DirectX::XMFLOAT3& p_direction, const float& p_range) = 0;
        };
    }
}