#pragma once
#include <DoremiEngine/Physics/Include/FluidManager.hpp>

namespace DoremiEngine
{
    namespace Graphic
    {
        class MeshInfo;
        class MaterialInfo;
    }
}

namespace Doremi
{
    namespace Core
    {
        /**
        Particle system which spews particle in a given direction, with an angle, more or less*/
        struct ParticlePressureComponent
        {
            // Data struct found in FluidManager. Sets all relevant parameters for the particle system
            DoremiEngine::Physics::ParticleEmitterData data;
            // The mesh of each particle
            DoremiEngine::Graphic::MeshInfo* mesh;
            // The material of each particle
            DoremiEngine::Graphic::MaterialInfo* material;
        };
    }
}