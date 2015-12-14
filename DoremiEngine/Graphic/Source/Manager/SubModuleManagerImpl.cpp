#pragma once
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <Internal/Manager/MeshManagerImpl.hpp>
#include <Internal/Manager/ParticleSystemManagerImpl.hpp>
#include <Internal/Manager/ShaderManagerImpl.hpp>
#include <Internal/Manager/Texture2DManagerImpl.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <Internal/Manager/CameraManagerImpl.hpp>
#include <Internal/Manager/LightManagerImpl.hpp>
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
            m_cameraManager->Initialize();

            // TODOKO Should not be here!! or should it? For standard shaders? Maybee in shadermanager

            D3D11_INPUT_ELEMENT_DESC ied[] = {
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
                {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
            };
            VertexShader* shader = m_shaderManager->BuildVertexShader("BasicVertexShader.hlsl", ied, ARRAYSIZE(ied));
            PixelShader* pshader = m_shaderManager->BuildPixelShader("BasicPixelShader.hlsl");
            m_shaderManager->SetActivePixelShader(pshader);
            m_shaderManager->SetActiveVertexShader(shader);
        }

        MeshManager& SubModuleManagerImpl::GetMeshManager() { return *m_meshManager; }

        ParticleSystemManager& SubModuleManagerImpl::GetParticleSystemManager() { return *m_particleSystemManager; }

        ShaderManager& SubModuleManagerImpl::GetShaderManager() { return *m_shaderManager; }

        Texture2DManager& SubModuleManagerImpl::GetTexuter2DManager() { return *m_texture2DManager; }

        DirectXManager& SubModuleManagerImpl::GetDirectXManager() { return *m_directXManager; }

        CameraManager& SubModuleManagerImpl::GetCameraManager() { return *m_cameraManager; }

        LightManager& SubModuleManagerImpl::GetLightManager() { return *m_lightManager; }
    }
}