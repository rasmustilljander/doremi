#pragma once
#include <Internal/Animation/SkeletalAnimationImpl.hpp>
#include <Internal/Animation/SkeletalInformation.hpp>
#include <string>

namespace DoremiEngine
{
    namespace Graphic
    {
        SkeletalAnimationImpl::SkeletalAnimationImpl() {}
        SkeletalAnimationImpl::~SkeletalAnimationImpl() {}

        void SkeletalAnimationImpl::GetFinalTransforms(const std::string& p_clipName, float t_timePos,
                                                       std::vector<DirectX::XMFLOAT4X4>& p_finalTransforms, SkeletalInformation* p_skeletalInformation) const
        {
            using namespace DirectX;
            // Get number of bones in the skeleton.
            unsigned int t_numBones = p_skeletalInformation->GetBoneCount();
            std::vector<XMFLOAT4X4> t_toParentTransforms(t_numBones);

            // Interpolate all the bones of this clip at the current time
            AnimationClip clip = p_skeletalInformation->GetAnimationClip(p_clipName);
            clip.Interpolate(t_timePos, t_toParentTransforms);

            // Traverse the heirarchy and transofrm all the bones to the root space
            std::vector<XMFLOAT4X4> t_toRootTransforms(t_numBones);
            // the root bone has index 0. The root bone has no parent. So the "parent transform" is just it's local transform
            t_toRootTransforms[0] = t_toParentTransforms[0];

            // Now traverse the tree top to bottom style and set the values of the vector of parent to root
            for(size_t i = 1; i < t_numBones; i++)
            {
                // Get current matrix
                XMMATRIX t_toParent = XMLoadFloat4x4(&t_toParentTransforms[i]);

                // Get parentIndex and the parent matrix (Parentmatrix is already updated to root)
                int parentIndex = p_skeletalInformation->GetParentIndex(i);
                XMMATRIX t_parentToRoot = XMLoadFloat4x4(&t_toRootTransforms[parentIndex]);

                // Multiply and store
                XMMATRIX t_toRoot = XMMatrixMultiply(t_toParent, t_parentToRoot);

                XMStoreFloat4x4(&t_toRootTransforms[i], t_toRoot);
            }

            // two solutions below. Use the first if we export maya with individual transformMatrices
            p_finalTransforms = t_toRootTransforms;

            // Premultiply by the bone offset transform to get the final transform (Wont be needed if we export maya with individual
            // transformmatrices.
            std::vector<XMFLOAT4X4> t_boneOffsets = p_skeletalInformation->GetBoneOffsets();
            for(size_t i = 0; i < t_numBones; i++)
            {
                XMMATRIX t_offset = XMLoadFloat4x4(&t_boneOffsets[i]);
                XMMATRIX t_toRoot = XMLoadFloat4x4(&t_toRootTransforms[i]);
                XMStoreFloat4x4(&p_finalTransforms[i], XMMatrixMultiply(t_offset, t_toRoot));
            }
        }
    }
}