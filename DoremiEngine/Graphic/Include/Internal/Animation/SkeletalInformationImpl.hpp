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
        // struct KeyFrame
        //{
        //    KeyFrame() : time(0.0f), position(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), quaternion(0.0f, 0.0f, 0.0f, 1.0f) {}
        //    ~KeyFrame() {}

        //    float time; // Kanske inte behövs. Räcker väl med frame å #framespersecond
        //    DirectX::XMFLOAT3 position;
        //    DirectX::XMFLOAT3 scale;
        //    DirectX::XMFLOAT4 quaternion;
        //};

        // struct BoneAnimation
        //{
        //    float GetStartTime() const
        //    {
        //        float f = Keyframes.front().time;
        //        return f;
        //    }
        //    float BoneAnimation::GetEndTime() const
        //    {
        //        float f = Keyframes.back().time;
        //        return f;
        //    }

        //    void BoneAnimation::Interpolate(float p_time, DirectX::XMFLOAT4X4& p_matrix) const
        //    {
        //        using namespace DirectX;
        //        // First check if time is below first frame. If so use first frame
        //        if (p_time <= Keyframes.front().time)
        //        {
        //            XMVECTOR t_scale = XMLoadFloat3(&Keyframes.front().scale);
        //            XMVECTOR t_position = XMLoadFloat3(&Keyframes.front().position);
        //            XMVECTOR t_quaternion = XMLoadFloat4(&Keyframes.front().quaternion);

        //            XMVECTOR t_zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        //            XMStoreFloat4x4(&p_matrix, XMMatrixAffineTransformation(t_scale, t_zero, t_quaternion, t_position));
        //        }
        //        // Check if we are after or at the last frame If so use the last frame.
        //        else if (p_time >= Keyframes.back().time)
        //        {
        //            XMVECTOR t_scale = XMLoadFloat3(&Keyframes.back().scale);
        //            XMVECTOR t_position = XMLoadFloat3(&Keyframes.back().position);
        //            XMVECTOR t_quaternion = XMLoadFloat4(&Keyframes.back().quaternion);

        //            XMVECTOR t_zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        //            XMStoreFloat4x4(&p_matrix, XMMatrixAffineTransformation(t_scale, t_zero, t_quaternion, t_position));
        //        }
        //        // If we are in the middle of the animation. We need to interpolate between frames most likely
        //        else
        //        {
        //            size_t length = Keyframes.size() - 1;
        //            for (size_t i = 0; i < length; i++)
        //            {
        //                // Find the correct place in the array by using time for each frame and the current timestamp.
        //                if (p_time >= Keyframes[i].time && p_time <= Keyframes[i + 1].time)
        //                {
        //                    // get a value between 0 and 1 to get the interpolation value.
        //                    float t_lerpPercent = (p_time - Keyframes[i].time) / (Keyframes[i + 1].time - Keyframes[i].time);

        //                    // Loadfloats to vectors to be able to use the Lerpfunction take the value that is just before the timestamp and the one
        //                    after

        //                    XMVECTOR t_scale0 = XMLoadFloat3(&Keyframes[i].scale);
        //                    XMVECTOR t_scale1 = XMLoadFloat3(&Keyframes[i + 1].scale);

        //                    XMVECTOR t_position0 = XMLoadFloat3(&Keyframes[i].position);
        //                    XMVECTOR t_position1 = XMLoadFloat3(&Keyframes[i + 1].position);

        //                    XMVECTOR t_quaternion0 = XMLoadFloat4(&Keyframes[i].quaternion);
        //                    XMVECTOR t_quaternion1 = XMLoadFloat4(&Keyframes[i + 1].quaternion);

        //                    // Use vectorlerp to interpolate between two vectors
        //                    XMVECTOR t_interpolatedScale = XMVectorLerp(t_scale0, t_scale1, t_lerpPercent);
        //                    XMVECTOR t_interpolatedPosition = XMVectorLerp(t_position0, t_position1, t_lerpPercent);
        //                    XMVECTOR t_interpolatedQuaternion = XMVectorLerp(t_quaternion0, t_quaternion1, t_lerpPercent);

        //                    // Get a zeroVector for rotation orientation.
        //                    XMVECTOR t_zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        //                    // Store the matrix in the inparameter. Use the XMMatrixAffineTransformation to get a matrix from the vectors we have
        //                    created.
        //                    XMStoreFloat4x4(&p_matrix, XMMatrixAffineTransformation(t_interpolatedScale, t_zero, t_interpolatedQuaternion,
        //                    t_interpolatedPosition));

        //                    break;
        //                }
        //            }
        //        }
        //    }

        //    std::vector<KeyFrame> Keyframes;
        //};

        // struct AnimationClip
        //{
        //    float AnimationClip::GetClipStartTime() const
        //    {
        //        // Find smallest starttime over all bones in this clip.
        //        float t_smallestTime = 99999999;
        //        size_t length = BoneAnimations.size();
        //        for (size_t i = 0; i < length; i++)
        //        {
        //            t_smallestTime = __min(t_smallestTime, BoneAnimations[i].GetStartTime());
        //        }
        //        return t_smallestTime;
        //    }

        //    float AnimationClip::GetClipEndTime() const
        //    {
        //        // find larges endtime over all bones in this clip.
        //        float t_largestTime = 0.0f;
        //        size_t length = BoneAnimations.size();
        //        for (size_t i = 0; i < length; i++)
        //        {
        //            t_largestTime = __max(t_largestTime, BoneAnimations[i].GetEndTime());
        //        }
        //        return t_largestTime;
        //    }

        //    void AnimationClip::Interpolate(float p_time, std::vector<DirectX::XMFLOAT4X4>& p_boneTransforms) const
        //    {
        //        size_t length = BoneAnimations.size();
        //        // interpolate all the bonetransforms
        //        for (size_t i = 0; i < length; i++)
        //        {
        //            BoneAnimations[i].Interpolate(p_time, p_boneTransforms[i]);
        //        }
        //    }

        //    std::vector<BoneAnimation> BoneAnimations;
        //};

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