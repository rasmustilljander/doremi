#pragma once

namespace DoremiEngine
{
    namespace Graphic
    {
        class MeshManager;
        class ParticleSystemManager;
        class ShaderManager;
        class Texture2DManager;
        class DirectXManager;
        class SubModuleManager
        {
        public:
            virtual void Initialize() = 0;
            virtual MeshManager& GetMeshManager() = 0;
            virtual ParticleSystemManager& GetParticleSystemManager() = 0;
            virtual ShaderManager& GetShaderManager() = 0;
            virtual Texture2DManager& GetTexuter2DManager() = 0;
            virtual DirectXManager& GetDirectXManager() = 0;
        };
    }
}