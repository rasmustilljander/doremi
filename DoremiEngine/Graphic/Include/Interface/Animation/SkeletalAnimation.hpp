#pragma once
#include <string>
#include <DirectXMath.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        /**
        This class is supposed to keep track of all the meshes' animations
        */
        class SkeletalInformation;
        class SkeletalAnimation
        {
        public:
            virtual void GetFinalTransforms(const std::string& p_clipName, float t_timePos, std::vector<DirectX::XMFLOAT4X4>& p_finalTransforms,
                                            SkeletalInformation* p_skeletalInformation) const = 0;
        };
    }
}