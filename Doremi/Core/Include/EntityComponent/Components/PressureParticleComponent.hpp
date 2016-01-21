#pragma once
#include <DoremiEngine/Physics/Include/FluidManager.hpp>
namespace Doremi
{
    namespace Core
    {
        /**
        Particle system which spews particle in a given direction, with an angle, more or less*/
        struct ParticlePressureComponent
        {
            DoremiEngine::Physics::ParticleEmitterData data;
        };
    }
}