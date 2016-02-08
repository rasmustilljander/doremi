#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
namespace DoremiEngine
{
    namespace Graphic
    {
        class SkeletalInformation;
        class SkeletalAnimation;
        class SkeletalAnimationManager
        {
        public:
            virtual SkeletalInformation* CreateSkeletalInformation() = 0;
            virtual void GetFinalTransforms(const std::string& p_clipName, float t_timePos, std::vector<DirectX::XMFLOAT4X4>& p_finalTransforms,
                                            SkeletalInformation* p_skeletalInformation) const = 0;
            virtual void PushMatricesToDevice(std::vector<DirectX::XMFLOAT4X4> p_transformsTopush) = 0;
        };
    }
}