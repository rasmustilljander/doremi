#pragma once
#include <DirectXMath.h>
namespace Doremi
{
    namespace Core
    {
        /**
        The audio component contains the handle to the soundchannel and a handle to the sound
        */
        struct MovementComponent
        {
            DirectX::XMFLOAT3 direction;
            float forwardAcceleration;
            float rightAcceleration;
            float maxSpeed;
            MovementComponent(const DirectX::XMFLOAT3& p_direction, const float& p_forwardAcceleration, const float& p_rightAcceleration, const float& p_maxSpeed)
                : direction(p_direction), forwardAcceleration(p_forwardAcceleration), rightAcceleration(p_rightAcceleration), maxSpeed(p_maxSpeed)
            {
            }
            MovementComponent() : direction(DirectX::XMFLOAT3(0, 0, 0)), forwardAcceleration(0), rightAcceleration(0), maxSpeed(0) {}
        };
    }
}