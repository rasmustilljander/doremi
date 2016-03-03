#pragma once
#include <DirectXMath.h>
using namespace DirectX;
namespace Doremi
{
    namespace Core
    {
        /**
        Simple component in which to store the desired movement of a character controller
        for an update.*/
        struct MovementComponent
        {
            XMFLOAT3 movement = XMFLOAT3(0, 0, 0);
            /**
            The amount of units a unit can walk per second
            */
            float speed = 10;
        };
    }
}