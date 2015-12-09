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
        class DirectXManagerImpl;
        struct GraphicModuleContext;
        class SubModuleManagerImpl : public SubModuleManager
        {
        public:
            SubModuleManagerImpl(const GraphicModuleContext& p_graphicContext);
            virtual ~SubModuleManagerImpl();
            void Initialize() override;
            MeshManager& GetMeshManager() override;
            ParticleSystemManager& GetParticleSystemManager() override;
            ShaderManager& GetShaderManager() override;
            Texture2DManager& GetTexuter2DManager() override;
            DirectXManager& GetDirectXManager() override;
        private:
            const GraphicModuleContext& m_graphicContext;
            MeshManagerImpl* m_meshManager;
            ParticleSystemManagerImpl* m_particleSystemManager;
            ShaderManagerImpl* m_shaderManager;
            Texture2DManagerImpl* m_texture2DManager;
            DirectXManagerImpl* m_directXManager;
            
        };
    }
}