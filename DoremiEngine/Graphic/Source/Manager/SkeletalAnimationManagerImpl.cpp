#pragma once
#include <Internal/Manager/SkeletalAnimationManagerImpl.hpp>
#include <Internal/Animation/SkeletalInformationImpl.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <Internal/Manager/SubModuleManagerImpl.hpp>
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
                96 * sizeof(DirectX::XMFLOAT4X4); // TODOXX hardcoded value. This is max bones in a rig Matches the skeletalanimation vertexshader
            bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            m_graphicContext.m_graphicModule->GetSubModuleManager().GetDirectXManager().GetDevice()->CreateBuffer(&bd, NULL, &m_matricesBuffer);
        }
        SkeletalInformation* SkeletalAnimationManagerImpl::CreateSkeletalInformation()
        {
            SkeletalInformation* t_skeletalInformation = new SkeletalInformationImpl();
            return t_skeletalInformation;
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