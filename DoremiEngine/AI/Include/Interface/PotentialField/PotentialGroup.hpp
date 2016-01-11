#pragma once
#include <DirectXMath.h>
#include <vector>
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialFieldActor;
        /**
        Contains a group of actors that will effect eachother with charges. This implementation does not use a field and is suitable
        for fully dynamic groups of units.
        */
        class PotentialGroup
        {
            public:
            /**
            Adds a new actor that will influate the potential field
            */
            virtual void AddActor(PotentialFieldActor* p_newActor) = 0;
            /**
            Returns the direction of the force that the given position gets from the group.
            If actor is set that actor will be skipped when calculating force
            */
            virtual DirectX::XMFLOAT3 GetForceDirection(const DirectX::XMFLOAT3& p_unitPosition, const PotentialFieldActor* p_actor = nullptr) = 0;
        };
    }
}
