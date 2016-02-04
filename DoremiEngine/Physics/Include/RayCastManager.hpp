#pragma once
#include <DirectXMath.h>
using namespace DirectX;
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
            Returns the id of the body hit by the ray. Returns -1 if no hit occured. Range may not be negative or 0
            */
            virtual int CastRay(const DirectX::XMFLOAT3& p_origin, const DirectX::XMFLOAT3& p_direction, const float& p_range) = 0;

            /**
            Sweeps a given object in the desired direction and returns the first object hit by the sweep*/
            virtual int CastSweep(const XMFLOAT3& p_origin, XMFLOAT3& p_direction, const float& p_range) = 0;
        };
    }
}