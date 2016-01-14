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
            Adds a quad to the occupied list
            */
            virtual void AddOccupiedQuad(const DirectX::XMINT2& p_quad) = 0;
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
            /**
            Gets all the quads the actor is occupiing
            */
            virtual const std::vector<DirectX::XMINT2>& GetOccupiedQuads() const = 0;
            /**
            Returns true if the actor is static, false for dynamic
            */
            virtual const bool& IsStatic() const = 0;
            /**
            Returns the closest actor occupied quad to the given position, the range is not taken in to consideration
            */
            virtual const DirectX::XMINT2& GetClosestOccupied(const DirectX::XMFLOAT3 p_position) = 0;
        };
    }
}
