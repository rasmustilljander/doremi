#pragma once
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <Internal/Manager/MeshManagerImpl.hpp>
#include <Internal/Manager/ParticleSystemManagerImpl.hpp>
#include <Internal/Manager/ShaderManagerImpl.hpp>
#include <Internal/Manager/Texture2DManagerImpl.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <Internal/Manager/CameraManagerImpl.hpp>
#include <Internal/Manager/LightManagerImpl.hpp>
#include <Internal/Manager/ComputeShaderManagerImpl.hpp>
#include <Internal/Manager/SkeletalAnimationManagerImpl.hpp>
#include <GraphicModuleContext.hpp>
// DirectX stuff
// TODOKO Should not need directx here
#include <d3d11_1.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        SubModuleManagerImpl::SubModuleManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext) {}

        SubModuleManagerImpl::~SubModuleManagerImpl() {}

        void SubModuleManagerImpl::Initialize()
        {
            m_directXManager = new DirectXManagerImpl(m_graphicContext);
            m_directXManager->InitializeDirectX();
            m_meshManager = new MeshManagerImpl(m_graphicContext);
            m_particleSystemManager = new ParticleSystemManagerImpl(m_graphicContext);
            m_shaderManager = new ShaderManagerImpl(m_graphicContext);
            m_texture2DManager = new Texture2DManagerImpl(m_graphicContext);
            m_cameraManager = new CameraManagerImpl(m_graphicContext);
            m_lightManager = new LightManagerImpl(m_graphicContext);
            m_computeShaderManager = new ComputeShaderManagerImpl(m_graphicContext);
            m_skeletalInformationManager = new SkeletalAnimationManagerImpl(m_graphicContext);
            m_skeletalInformationManager->Initialize();
            m_cameraManager->Initialize();

            // TODOKO Should not be here!! or should it? For standard shaders? Maybee in shadermanager

            ComputeShader* cshader = m_shaderManager->BuildComputeShader("FrustumComputeShader.hlsl");
            //m_computeShaderManager->DispatchFrustum();
            // m_computeShaderManager->DispatchCulling();
        }

        MeshManager& SubModuleManagerImpl::GetMeshManager() { return *m_meshManager; }

        ParticleSystemManager& SubModuleManagerImpl::GetParticleSystemManager() { return *m_particleSystemManager; }

        ShaderManager& SubModuleManagerImpl::GetShaderManager() { return *m_shaderManager; }

        Texture2DManager& SubModuleManagerImpl::GetTexuter2DManager() { return *m_texture2DManager; }

        DirectXManager& SubModuleManagerImpl::GetDirectXManager() { return *m_directXManager; }

        CameraManager& SubModuleManagerImpl::GetCameraManager() { return *m_cameraManager; }

        LightManager& SubModuleManagerImpl::GetLightManager() { return *m_lightManager; }

        MeshManagerImpl& SubModuleManagerImpl::GetMeshManagerImpl() { return *m_meshManager; }

        ParticleSystemManagerImpl& SubModuleManagerImpl::GetParticleSystemManagerImpl() { return *m_particleSystemManager; }

        ShaderManagerImpl& SubModuleManagerImpl::GetShaderManagerImpl() { return *m_shaderManager; }

        Texture2DManagerImpl& SubModuleManagerImpl::GetTexuter2DManagerImpl() { return *m_texture2DManager; }

        DirectXManagerImpl& SubModuleManagerImpl::GetDirectXManagerImpl() { return *m_directXManager; }

        CameraManagerImpl& SubModuleManagerImpl::GetCameraManagerImpl() { return *m_cameraManager; }

        LightManagerImpl& SubModuleManagerImpl::GetLightManagerImpl() { return *m_lightManager; }

        ComputeShaderManager& SubModuleManagerImpl::GetComputeShaderManager() { return *m_computeShaderManager; }

        SkeletalAnimationManager& SubModuleManagerImpl::GetSkeletalAnimationManager() { return *m_skeletalInformationManager; }
    }
}