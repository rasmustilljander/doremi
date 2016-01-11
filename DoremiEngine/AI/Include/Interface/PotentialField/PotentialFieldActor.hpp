#pragma once
#include <DirectXMath.h>
#include <vector>
namespace DoremiEngine
{
    namespace AI
    {
        /**
        TODOKO docs
        */
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
            /**
            Sets the range of the actor
            */
            virtual void SetRange(const float& p_range) = 0;
            /**
            Sets if the actor is static or dynamic
            */
            virtual void SetStatic(const bool& p_static) = 0;
            /**
            Gets the actor position
            */
            virtual const DirectX::XMFLOAT3& GetPosition() const = 0;
            /**
            Gets the actors charge
            */
            virtual const float& GetCharge() const = 0;
            /**
            Gets the range of the actor
            */
            virtual const float& GetRange() const = 0;
        };
    }
}
