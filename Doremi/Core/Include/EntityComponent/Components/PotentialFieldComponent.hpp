#pragma once
#include <DirectXMath.h>
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
            decides both (-)repulsive and (+)atracting force. Needs a good initialization position to see what field it's in. TODOKO might not need pos in start later
            */
            DoremiEngine::AI::PotentialFieldActor* ChargedActor;
            /**
            Needs to be checked for nullptr error. The field the actor is supposed to use. Dont created new once here, just fetch the one that is closest from the potential field sub module
            */
            DoremiEngine::AI::PotentialField* Field;
            PotentialFieldComponent(DoremiEngine::AI::PotentialFieldActor* p_actor, DoremiEngine::AI::PotentialField* p_potentialField) : ChargedActor(p_actor), Field(p_potentialField){}
            PotentialFieldComponent() : ChargedActor(nullptr), Field(nullptr) {}
        };
    }
}