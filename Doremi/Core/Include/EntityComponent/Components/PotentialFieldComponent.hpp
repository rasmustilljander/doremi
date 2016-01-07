#pragma once
#include <DirectXMath.h>
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialFieldActor;
    }
}
namespace Doremi
{
    namespace Core
    {
        /**
        Contains information on the charge TODOKO add more needed things
        */
        struct PotentialFieldComponent
        {
            /**
            decides both (-)repulsive and (+)atracting force
            */
            DoremiEngine::AI::PotentialFieldActor* ChargedActor;

            PotentialFieldComponent(DoremiEngine::AI::PotentialFieldActor* p_actor) : ChargedActor(p_actor) {}
            PotentialFieldComponent() : ChargedActor(nullptr) {}
        };
    }
}