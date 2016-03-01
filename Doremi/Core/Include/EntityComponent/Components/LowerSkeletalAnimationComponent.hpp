#pragma once
#include <string>
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
        struct LowerSkeletalAnimationComponent
        {
            DoremiEngine::Graphic::SkeletalInformation* skeletalInformation;
            std::string clipName;
            double timePosition;
            DirectX::XMFLOAT4 orientation;
            SkeletalAnimationType type;
            LowerSkeletalAnimationComponent(DoremiEngine::Graphic::SkeletalInformation* p_skeletalInformation)
                : skeletalInformation(p_skeletalInformation)
            {
            }
            LowerSkeletalAnimationComponent() : skeletalInformation(nullptr) {}
        };
    }
}