#pragma once
#include <Interface/Manager/SubModuleManager.hpp>
namespace DoremiEngine
{

    namespace Graphic
    {
        class MeshManagerImpl;
        class ParticleSystemManagerImpl;
        class ShaderManagerImpl;
        class Texture2DManagerImpl;

        class SubModuleManagerImpl : public SubModuleManager
        {
        public:
            SubModuleManagerImpl();
            virtual ~SubModuleManagerImpl();
            MeshManager* GetMeshManager() override;
            ParticleSystemManager* GetParticleSystemManager() override;
            ShaderManager* GetShaderManager() override;
            Texture2DManager* GetTexuter2DManager() override;
        private:
            MeshManagerImpl* m_meshManager;
            ParticleSystemManagerImpl* m_particleSystemManager;
            ShaderManagerImpl* m_shaderManager;
            Texture2DManagerImpl* m_texture2DManager;
        };
    }
}