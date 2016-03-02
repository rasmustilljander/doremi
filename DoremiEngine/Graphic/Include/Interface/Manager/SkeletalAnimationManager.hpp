#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <map>
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
            /**
                Sets the values of the matrices that are sent into p_finalTransforms parameter. Finds the 2 keyframes closest to the timeposition and
               interpolates
            */
            virtual void GetFinalTransforms(const std::string& p_clipName, float t_timePos, std::vector<DirectX::XMFLOAT4X4>& p_finalTransforms,
                                            SkeletalInformation* p_skeletalInformation) const = 0;
            /**
                Puts the matrices into the gpu
            */
            virtual void PushMatricesToDevice(std::vector<DirectX::XMFLOAT4X4> p_transformsTopush) = 0;
            virtual void GetInterpolatedAnimationsMatrices(SkeletalInformation* p_skeletalInformation, std::string t_clipName, float t_timePos,
                                                           std::vector<DirectX::XMFLOAT4X4>& p_finalTransforms,
                                                           std::map<std::string, float> p_animationTransitions) const = 0;
        };
    }
}