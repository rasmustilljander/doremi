#pragma once
#include <DirectXMath.h>
#include <vector>
#include <map>
namespace DirectX
{
    struct XMFLOAT4X4;
}


namespace DoremiEngine
{
    namespace Graphic
    {
        struct KeyFrame
        {
            KeyFrame();
            ~KeyFrame();

            float time; // Kanske inte behövs. Räcker väl med frame å #framespersecond
            DirectX::XMFLOAT3 position;
            DirectX::XMFLOAT3 scale;
            DirectX::XMFLOAT4 quaternion;
        };

        struct BoneAnimation
        {
            float GetStartTime() const;
            float GetEndTime() const;

            void Interpolate(float t, DirectX::XMFLOAT4X4& M) const;

            std::vector<KeyFrame> Keyframes;
        };

        struct AnimationClip
        {
            float GetClipStartTime() const;

            float GetClipEndTime() const;

            void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& p_boneTransforms) const;

            std::vector<BoneAnimation> BoneAnimations;
        };

        class SkeletalInformation
        {
        public:
            // TODOLH add documents
            // Contains skeletalanimation information kinda like materialinfo
            SkeletalInformation();
            virtual ~SkeletalInformation();
            /**
                Sets the values of this class
            */
            void Set(std::vector<int>& p_boneHierarchy, std::vector<DirectX::XMFLOAT4X4>& p_boneOffsets, std::map<std::string, AnimationClip>& p_animations);

            unsigned int GetBoneCount() const;
            float GetClipStartTime(const std::string& clipName) const;
            float GetClipEndTime(const std::string& clipName) const;
            AnimationClip GetAnimationClip(const std::string& p_clipName) const;
            int GetParentIndex(const int& p_childIndex) const;
            std::vector<DirectX::XMFLOAT4X4> GetBoneOffsets() const;


        private:
            /**
                list of every bones parent. Index of the array represents the boneNR
            */
            std::vector<int> m_parentIndex;
            /**
                list of every bone's offset transformationmatrix
            */
            std::vector<DirectX::XMFLOAT4X4> m_boneOffsets;
            /**
                map of animationclips to their respective names
            */
            std::map<std::string, AnimationClip> m_animations;
        };
    }
}