#pragma once
#include <DirectXMath.h>
#include <vector>
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialFieldActor
        {
            public:
            /**
            Sets the actor position
            */
            virtual void SetPosition(const DirectX::XMFLOAT3& p_position) = 0;
            /**
            Sets the charge of the actor. + for attraction - for repulsion
            */
            virtual void SetCharge(const float& p_charge) = 0;
        };
    }
}
