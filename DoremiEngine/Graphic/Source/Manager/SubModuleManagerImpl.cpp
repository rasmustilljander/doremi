#pragma once
#include <Internal/Manager/SubModuleManagerImpl.hpp>
#include <Internal/Manager/MeshManagerImpl.hpp>
#include <Internal/Manager/ParticleSystemManagerImpl.hpp>
#include <Internal/Manager/ShaderManagerImpl.hpp>
#include <Internal/Manager/Texture2DManagerImpl.hpp>
namespace DoremiEngine
{
    namespace Graphic
    {
        SubModuleManagerImpl::SubModuleManagerImpl() 
        {
            m_meshManager = new MeshManagerImpl();
            m_particleSystemManager = new ParticleSystemManagerImpl();
            m_shaderManager = new ShaderManagerImpl();
            m_texture2DManager = new Texture2DManagerImpl();
        }

        SubModuleManagerImpl::~SubModuleManagerImpl() 
        {}

        MeshManager* SubModuleManagerImpl::GetMeshManager() 
        {
            return m_meshManager;
        }

        ParticleSystemManager* SubModuleManagerImpl::GetParticleSystemManager() 
        {
            return m_particleSystemManager;
        }

        ShaderManager* SubModuleManagerImpl::GetShaderManager() 
        {
            return m_shaderManager;
        }

        Texture2DManager* SubModuleManagerImpl::GetTexuter2DManager() 
        {
            return m_texture2DManager;
        }
    }
}