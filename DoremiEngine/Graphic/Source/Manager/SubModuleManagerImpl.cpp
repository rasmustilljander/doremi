#pragma once
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <Internal/Manager/MeshManagerImpl.hpp>
#include <Internal/Manager/ParticleSystemManagerImpl.hpp>
#include <Internal/Manager/ShaderManagerImpl.hpp>
#include <Internal/Manager/Texture2DManagerImpl.hpp>
#include <Internal/Manager/DirectXManagerImpl.hpp>
#include <GraphicModuleContext.hpp>
//DirectX stuff

namespace DoremiEngine
{
    namespace Graphic
    {
        SubModuleManagerImpl::SubModuleManagerImpl(const GraphicModuleContext& p_graphicContext) : m_graphicContext(p_graphicContext)
        {

        }

        SubModuleManagerImpl::~SubModuleManagerImpl() 
        {}

        void SubModuleManagerImpl::Initialize()
        {
            m_directXManager = new DirectXManagerImpl(m_graphicContext);
            m_directXManager->InitializeDirectX();
            m_meshManager = new MeshManagerImpl(m_graphicContext);
            m_particleSystemManager = new ParticleSystemManagerImpl(m_graphicContext);
            m_shaderManager = new ShaderManagerImpl(m_graphicContext);
            m_texture2DManager = new Texture2DManagerImpl(m_graphicContext);
        }

        MeshManager& SubModuleManagerImpl::GetMeshManager() 
        {
            return* m_meshManager;
        }

        ParticleSystemManager& SubModuleManagerImpl::GetParticleSystemManager() 
        {
            return* m_particleSystemManager;
        }

        ShaderManager& SubModuleManagerImpl::GetShaderManager() 
        {
            return* m_shaderManager;
        }

        Texture2DManager& SubModuleManagerImpl::GetTexuter2DManager() 
        {
            return* m_texture2DManager;
        }

        DirectXManager& SubModuleManagerImpl::GetDirectXManager()
        {
            return* m_directXManager;
        }
    }
}