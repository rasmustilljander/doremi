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
        class CameraManager;
        class LightManager;
        class SubModuleManager
        {
            public:
            virtual MeshManager& GetMeshManager() = 0;
            virtual ParticleSystemManager& GetParticleSystemManager() = 0;
            virtual ShaderManager& GetShaderManager() = 0;
            virtual Texture2DManager& GetTexuter2DManager() = 0;
            virtual DirectXManager& GetDirectXManager() = 0;
            virtual CameraManager& GetCameraManager() = 0;
            virtual LightManager& GetLightManager() = 0;
        };
    }
}