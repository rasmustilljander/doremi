#pragma once
#include <DoremiEngine/Graphic/Include/Internal/Animation/SkeletalInformationImpl.hpp>
#include <string>

namespace DoremiEngine
{
    namespace Graphic
    {
        // Define the SkeletalInformation functions

        SkeletalInformationImpl::SkeletalInformationImpl() {}
        SkeletalInformationImpl::~SkeletalInformationImpl() {}

        float SkeletalInformationImpl::GetClipStartTime(const std::string& p_clipName) const
        {
            auto clip = m_animations.find(p_clipName);
            return clip->second.GetClipStartTime();
        }

        float SkeletalInformationImpl::GetClipEndTime(const std::string& p_clipName) const
        {
            auto clip = m_animations.find(p_clipName);
            return clip->second.GetClipEndTime();
        }

        void SkeletalInformationImpl::Set(std::vector<int>& p_boneHierarchy, /* std::vector<DirectX::XMFLOAT4X4>& p_boneOffsets,*/
                                          std::map<std::string, AnimationClip>& p_animations)
        {
            m_parentIndex = p_boneHierarchy;
            // m_boneOffsets = p_boneOffsets;
            m_animations = p_animations;
        }
        unsigned int SkeletalInformationImpl::GetBoneCount() const { return m_parentIndex.size(); }
        AnimationClip SkeletalInformationImpl::GetAnimationClip(const std::string& p_clipName) const
        {
            auto clip = m_animations.find(p_clipName);
            return clip->second;
        }
        int SkeletalInformationImpl::GetParentIndex(const int& p_childIndex) const { return m_parentIndex[p_childIndex]; }
        // std::vector<DirectX::XMFLOAT4X4> SkeletalInformation::GetBoneOffsets() const { return m_boneOffsets; }
    }
}