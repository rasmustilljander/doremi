#pragma once
#include <DirectXMath.h>

namespace DoremiEngine
{
    namespace AI
    {
        /**
        Is in charge of creating different potential field/group related objects
        */
        class PotentialField;
        class PotentialGroup;
        class PotentialFieldActor;
        class PotentialFieldSubModule
        {
        public:
            /**
            Creates a new potential field with no actors
            */
            virtual PotentialField* CreateNewField(const float& p_width, const float& p_height, const int& p_numberOfQuadsWidth,
                                                   const int& p_numberOfQuadsHeight, const DirectX::XMFLOAT3& p_center) = 0;
            /**
            Creates a new potential group with no actors
            */
            virtual PotentialGroup* CreateNewPotentialGroup() = 0;
            /**
            Creates a new actor
            */
            virtual PotentialFieldActor* CreateNewActor(const DirectX::XMFLOAT3& p_position, const float& p_charge, const float& p_range, const bool& p_static) = 0;
            /**
            Attaches a actor to the given potentialfield. This can be done by going straight to the potential field to
            TODOKO review if it should be doable in this class or only in the field
            */
            virtual void AttachActor(PotentialField& o_field, PotentialFieldActor* p_actor) = 0;
            /**
            removes a actor from the given potentialfield TODOKO add func if needed
            */

            virtual int GetNextStep() = 0;
        };
    }
}
