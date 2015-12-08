#pragma once

namespace DoremiEngine
{
    namespace Graphic
    {
        class MeshManager;
        class ParticleSystemManager;
        class ShaderManager;
        class Texture2DManager;
        class SubModuleManager
        {
        public:
            virtual MeshManager* GetMeshManager() = 0;
            virtual ParticleSystemManager* GetParticleSystemManager() = 0;
            virtual ShaderManager* GetShaderManager() = 0;
            virtual Texture2DManager* GetTexuter2DManager() = 0;
        };
    }
}