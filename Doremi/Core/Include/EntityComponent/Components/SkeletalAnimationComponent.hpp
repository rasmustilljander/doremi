#pragma once
#include <string>
#include <map>
#include <DirectXMath.h>
#include <Doremi/Core/Include/Helper/AnimationStates.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        class SkeletalInformation;
    }
}

namespace Doremi
{
    namespace Core
    {
        /**
        Contains information of the skeletalanimation
        */

        struct SkeletalAnimationComponent
        {
            DoremiEngine::Graphic::SkeletalInformation* skeletalInformation;
            std::string clipName;
            double timePosition;
            SkeletalAnimationType type;
            std::map<std::string, float>* animationTransitions;
            SkeletalAnimationComponent(DoremiEngine::Graphic::SkeletalInformation* p_skeletalInformation) : skeletalInformation(p_skeletalInformation)
            {
            }
            SkeletalAnimationComponent() : skeletalInformation(nullptr) {}
        };
    }
}