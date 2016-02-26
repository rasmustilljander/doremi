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
            return clip->second.animationClip.GetClipStartTime();
        }

        float SkeletalInformationImpl::GetClipEndTime(const std::string& p_clipName) const
        {
            auto clip = m_animations.find(p_clipName);
            return clip->second.animationClip.GetClipEndTime();
        }

        void SkeletalInformationImpl::Set(std::vector<int>& p_boneHierarchy, std::map<std::string, AnimationBlend>& p_animations)
        {
            m_parentIndex = p_boneHierarchy;
            m_animations = p_animations;
        }
        unsigned int SkeletalInformationImpl::GetBoneCount() const { return m_parentIndex.size(); }
        AnimationClip SkeletalInformationImpl::GetAnimationClip(const std::string& p_clipName) const
        {
            auto clip = m_animations.find(p_clipName);
            return clip->second.animationClip;
        }
        int SkeletalInformationImpl::GetParentIndex(const int& p_childIndex) const { return m_parentIndex[p_childIndex]; }
        AnimationBlend& SkeletalInformationImpl::GetAnimationBlend(const std::string& p_clipName)
        {
            auto& clip = m_animations.find(p_clipName);
            return clip->second;
        }
        std::vector<std::string> SkeletalInformationImpl::GetAnimationNames() const
        {
            unsigned int animationCount = m_animations.size();
            std::vector<std::string> r_vector;
            for(auto animationblend : m_animations)
            {
                r_vector.push_back(animationblend.first);
            }
            return r_vector;
        }
    }
}