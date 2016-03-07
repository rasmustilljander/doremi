#pragma once
#include <DirectXMath.h>

/// Engine
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialFieldActor.hpp>
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialFieldActor;
        class PotentialField;
    }
}
namespace Doremi
{
    namespace Core
    {
        /**
        Contains information on the charge and the field the actor is in
        */
        struct PotentialFieldComponent
        {
            /**
            decides both (-)repulsive and (+)atracting force. Needs a good initialization position to see what field it's in. TODOKO might not need
            pos in start later
            */
            DoremiEngine::AI::PotentialFieldActor* ChargedActor;
            /**
            Needs to be checked for nullptr error. The field the actor is supposed to use. Dont created new once here, just fetch the one that is
            closest from the potential field sub module
            */
            DoremiEngine::AI::PotentialField* Field;
            /**
            States wheter or not this is a field, in case of true this entity will update the fields position. This shouldnt be a actor if true...
            If the field is to move around it might not be a good idea to have obstacles on it.
            */
            bool isField = false;
            PotentialFieldComponent(DoremiEngine::AI::PotentialFieldActor* p_actor, DoremiEngine::AI::PotentialField* p_potentialField)
                : ChargedActor(p_actor), Field(p_potentialField), isStatic(false)
            {
            }
            PotentialFieldComponent() : ChargedActor(nullptr), Field(nullptr), isStatic(false) {}

            // TODOJB confirm standard values and add docs
            float charge = 0;
            float range = 0;
            bool isStatic;
            DoremiEngine::AI::AIActorType type;
        };
    }
}