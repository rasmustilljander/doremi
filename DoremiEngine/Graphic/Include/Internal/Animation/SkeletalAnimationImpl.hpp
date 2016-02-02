#pragma once
#include <Interface/Animation/SkeletalAnimation.hpp>
#include <DirectXMath.h>
#include <string>
#include <vector>
namespace DirectX
{
    struct XMFLOAT4X4;
}
namespace DoremiEngine
{
    namespace Graphic
    {
        class SkeletalInformation;

        class SkeletalAnimationImpl : public SkeletalAnimation
        {
        public:
            // TODOLH add documents
            SkeletalAnimationImpl();
            virtual ~SkeletalAnimationImpl();

            /**
            CHanges the matrices sent into the p_finalTransforms parameter to the final transforms for every bone in the skeleton
            */
            void GetFinalTransforms(const std::string& p_clipName, float t_timePos, std::vector<DirectX::XMFLOAT4X4>& p_finalTransforms,
                                    SkeletalInformation* p_skeletalInformation) const override;

        private:
        };
    }
}