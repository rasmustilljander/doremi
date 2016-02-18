#pragma once
#include <DirectXMath.h>
#include <vector>
namespace DoremiEngine
{
    namespace AI
    {
        /**
        Is in charge of creating different potential field/group related objects and global potential field management
        */
        class PotentialField;
        class PotentialGroup;
        class PotentialFieldActor;
        enum class AIActorType : size_t;
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
            virtual PotentialFieldActor* CreateNewActor(const DirectX::XMFLOAT3& p_position, const float& p_charge, const float& p_range,
                                                        const bool& p_static, const AIActorType& p_actorType) = 0;
            /**
            Attaches a actor to the given potentialfield. This can be done by going straight to the potential field to
            */
            virtual void AttachActor(PotentialField& o_field, PotentialFieldActor* p_actor) = 0;

            /**
            Removes a actor from every field it's in and also deletes the actor
            */
            virtual void EraseActor(PotentialFieldActor* op_actor) = 0;

            /**
            Removes a actor from the given field also deletes the actor
            */
            virtual void EraseActor(PotentialFieldActor* op_actor, PotentialField* op_field) = 0;

            /**
            Finds the potential field that the given position probably should be in. Returns nullptr if no field were found
            */
            virtual PotentialField* FindBestPotentialField(const DirectX::XMFLOAT3& p_position) = 0;

            /**
            Adds the given actor to all active fields
            */
            virtual void AddActorToEveryPotentialField(PotentialFieldActor* p_actor) = 0;

            /**
            Returns a vector with pointers to all active fields.
            */
            virtual std::vector<PotentialField*>& GetAllActiveFields() = 0;
        };
    }
}
