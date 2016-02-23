#pragma once
#define NOMINMAX
#include <DirectXMath.h>
#include <DoremiEngine/Graphic/Include/Interface/Animation/SkeletalInformation.hpp>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>
namespace DirectX
{
    struct XMFLOAT4X4;
}


namespace DoremiEngine
{
    namespace Graphic
    {
        class SkeletalInformationImpl : public SkeletalInformation
        {
        public:
            // TODOLH add documents
            // Contains skeletalanimation information kinda like materialinfo
            SkeletalInformationImpl();
            virtual ~SkeletalInformationImpl();
            /**
                Sets the values of this class
            */
            void Set(std::vector<int>& p_boneHierarchy,
                     /*std::vector<DirectX::XMFLOAT4X4>& p_boneOffsets,*/ std::map<std::string, AnimationClip>& p_animations) override;

            unsigned int GetBoneCount() const override;
            float GetClipStartTime(const std::string& clipName) const override;
            float GetClipEndTime(const std::string& clipName) const override;
            AnimationClip GetAnimationClip(const std::string& p_clipName) const override;
            int GetParentIndex(const int& p_childIndex) const override;
            // std::vector<DirectX::XMFLOAT4X4> GetBoneOffsets() const;


        private:
            /**
                list of every bones parent. Index of the array represents the boneNR
            */
            std::vector<int> m_parentIndex;
            /**
                list of every bone's offset transformationmatrix
            */
            // std::vector<DirectX::XMFLOAT4X4> m_boneOffsets;
            /**
                map of animationclips to their respective names
            */
            std::map<std::string, AnimationClip> m_animations;
        };
    }
}