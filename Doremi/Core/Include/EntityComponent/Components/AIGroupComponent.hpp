#pragma once
#include <DirectXMath.h>
namespace DoremiEngine
{
    namespace AI
    {
        class PotentialGroup;
    }
}
namespace Doremi
{
    namespace Core
    {
        /**
        Contains the group the AI unit should use for path finding
        */
        struct AIGroupComponent
        {
            DoremiEngine::AI::PotentialGroup* Group;

            explicit AIGroupComponent(DoremiEngine::AI::PotentialGroup* p_group) : Group(p_group) {}
            AIGroupComponent() : Group(nullptr) {}
        };
    }
}