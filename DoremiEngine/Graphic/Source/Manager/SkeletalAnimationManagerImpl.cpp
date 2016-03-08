#pragma once
#include <Internal/Manager/SkeletalAnimationManagerImpl.hpp>
#include <Internal/Animation/SkeletalInformationImpl.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <Doremi/Core/Include/Helper/AnimationStates.hpp>
#include <GraphicModuleImplementation.hpp>
#include <GraphicModuleContext.hpp>
// DirectX stuff
// TODOKO Should not need directx here
#include <d3d11_1.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        SkeletalAnimationManagerImpl::SkeletalAnimationManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext)
        {
        }

        SkeletalAnimationManagerImpl::~SkeletalAnimationManagerImpl() {}

        void SkeletalAnimationManagerImpl::Initialize()
        {
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DYNAMIC;
            bd.ByteWidth =
                40 * sizeof(DirectX::XMFLOAT4X4); // TODOXX hardcoded value. This is max bones in a rig Matches the skeletalanimation vertexshader
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager().GetDevice()->CreateBuffer(&bd, NULL, &m_matricesBuffer);
        }
        SkeletalInformation* SkeletalAnimationManagerImpl::CreateSkeletalInformation()
        {
            SkeletalInformation* t_skeletalInformation = new SkeletalInformationImpl();
            return t_skeletalInformation;
        }


        void SkeletalAnimationManagerImpl::InterpolateBlending(std::vector<DirectX::XMFLOAT4X4>& o_finalTransforms,
                                                               std::vector<InterpolatedVectors> p_interpolatedVector) const
        {
            using namespace DirectX;
            size_t t_numberOfVectors = p_interpolatedVector.size();
            size_t t_numberOfBones = p_interpolatedVector[0].position.size();
            InterpolatedVectors t_currentAnimation = p_interpolatedVector[t_numberOfVectors - 1];
            float t_lerpPercent;
            for(size_t i = 0; i < t_numberOfVectors; i++)
            {
                t_lerpPercent = p_interpolatedVector[i].timeElapsed / 0.5;
                for(size_t k = 0; k < t_numberOfBones; k++)
                {
                    XMStoreFloat3(&t_currentAnimation.position[k], XMVectorLerp(XMLoadFloat3(&t_currentAnimation.position[k]),
                                                                                XMLoadFloat3(&p_interpolatedVector[i].position[k]), t_lerpPercent));
                    XMStoreFloat3(&t_currentAnimation.scale[k],
                                  XMVectorLerp(XMLoadFloat3(&t_currentAnimation.scale[k]), XMLoadFloat3(&p_interpolatedVector[i].scale[k]), t_lerpPercent));
                    XMStoreFloat4(&t_currentAnimation.rotation[k], XMQuaternionSlerp(XMLoadFloat4(&t_currentAnimation.rotation[k]),
                                                                                     XMLoadFloat4(&p_interpolatedVector[i].rotation[k]), t_lerpPercent));
                }
            }
            XMVECTOR zero = XMLoadFloat4(&XMFLOAT4(0, 0, 0, 1));
            for(size_t i = 0; i < t_numberOfBones; i++)
            {
                XMVECTOR scale = XMLoadFloat3(&t_currentAnimation.scale[i]);
                XMVECTOR position = XMLoadFloat3(&t_currentAnimation.position[i]);
                XMVECTOR rotation = XMLoadFloat4(&t_currentAnimation.rotation[i]);
                XMStoreFloat4x4(&o_finalTransforms[i], XMMatrixAffineTransformation(scale, zero, rotation, position));
            }
        }

        void SkeletalAnimationManagerImpl::GetInterpolatedAnimationsMatrices(SkeletalInformation* p_skeletalInformation, std::string t_clipName,
                                                                             float t_timePos, std::vector<DirectX::XMFLOAT4X4>& p_finalTransforms,
                                                                             std::map<std::string, Doremi::Core::AnimationTransitionInfo> p_animationTransitions) const
        {
            using namespace DirectX;
            // Get number of bones in the skeleton.
            unsigned int t_numBones = p_skeletalInformation->GetBoneCount();
            std::vector<XMFLOAT4X4> t_toParentTransforms(t_numBones);
            std::vector<std::string> t_animationNames = (p_skeletalInformation->GetAnimationNames());
            size_t t_animationLenght = t_animationNames.size();
            // Interpolate all the clips that have the timer started
            std::vector<InterpolatedVectors> t_interpolatedVectors;
            InterpolatedVectors t_currentAnimation;
            t_currentAnimation.position.resize(t_numBones);
            t_currentAnimation.scale.resize(t_numBones);
            t_currentAnimation.rotation.resize(t_numBones);
            for(size_t i = 0; i < t_animationLenght; i++)
            {
                if(p_animationTransitions[t_animationNames[i]].startTime > 0 && t_animationNames[i] != t_clipName)
                {
                    float t_timeElapsed = p_animationTransitions[t_animationNames[i]].elapsedSinceStart - p_animationTransitions[t_animationNames[i]].startTime;
                    InterpolatedVectors t_vectors;
                    t_vectors.position.resize(t_numBones);
                    t_vectors.scale.resize(t_numBones);
                    t_vectors.rotation.resize(t_numBones);
                    p_skeletalInformation->GetAnimationClip(t_animationNames[i])
                        .InterpolateBlend(t_timeElapsed, t_vectors.position, t_vectors.scale, t_vectors.rotation);
                    t_vectors.timeElapsed = t_timeElapsed;
                    t_interpolatedVectors.push_back(t_vectors);
                }
                else if(t_animationNames[i] == t_clipName)
                {
                    p_skeletalInformation->GetAnimationClip(t_animationNames[i])
                        .InterpolateBlend(t_timePos, t_currentAnimation.position, t_currentAnimation.scale, t_currentAnimation.rotation);
                    t_currentAnimation.timeElapsed = 0;
                }
            }
            t_interpolatedVectors.push_back(t_currentAnimation);
            InterpolateBlending(t_toParentTransforms, t_interpolatedVectors);


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
                // XMMATRIX t_toRoot = XMMatrixMultiply(t_parentToRoot, t_toParent);

                XMMATRIX t_toRoot = XMMatrixMultiply(t_toParent, t_parentToRoot); // Original

                XMStoreFloat4x4(&t_toRootTransforms[i], t_toRoot);
            }
            p_finalTransforms = t_toRootTransforms;
        }
        void SkeletalAnimationManagerImpl::GetFinalTransforms(const std::string& p_clipName, float p_timePos, std::vector<DirectX::XMFLOAT4X4>& p_finalTransforms,
                                                              SkeletalInformation* p_skeletalInformation) const
        {
            using namespace DirectX;
            // Get number of bones in the skeleton.
            unsigned int t_numBones = p_skeletalInformation->GetBoneCount();
            std::vector<XMFLOAT4X4> t_toParentTransforms(t_numBones);

            // Interpolate all the bones of this clip at the current time
            AnimationClip clip = p_skeletalInformation->GetAnimationClip(p_clipName);
            clip.Interpolate(p_timePos, t_toParentTransforms);


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
                // XMMATRIX t_toRoot = XMMatrixMultiply(t_parentToRoot, t_toParent);

                XMMATRIX t_toRoot = XMMatrixMultiply(t_toParent, t_parentToRoot); // Original

                XMStoreFloat4x4(&t_toRootTransforms[i], t_toRoot);
            }
            p_finalTransforms = t_toRootTransforms;
        }
        void SkeletalAnimationManagerImpl::PushMatricesToDevice(std::vector<DirectX::XMFLOAT4X4> p_transformsToPush)
        {
            using namespace DirectX;
            DirectXManager& m_directX = m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager();
            D3D11_MAPPED_SUBRESOURCE tMS;
            // Transpose all the matrices
            for(size_t i = 0; i < p_transformsToPush.size(); i++)
            {
                XMStoreFloat4x4(&p_transformsToPush[i], XMMatrixTranspose(XMLoadFloat4x4(&p_transformsToPush[i])));
            }
            // Send to gpu
            m_directX.GetDeviceContext()->Map(m_matricesBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &tMS);
            memcpy(tMS.pData, p_transformsToPush.data(), sizeof(DirectX::XMFLOAT4X4) * p_transformsToPush.size());
            m_directX.GetDeviceContext()->Unmap(m_matricesBuffer, NULL);
            m_directX.GetDeviceContext()->VSSetConstantBuffers(2, 1, &m_matricesBuffer);
        }
    }
}