#pragma once
#include <vector>
#include <DirectXMath.h>
using namespace std;
using namespace DirectX;
namespace DoremiEngine
{
    namespace Physics
    {
        /**
        This is used when creating particle emitters
        Ensure that ALL variables are filled. NO
        standard values are assigned. YOU HAVE BEEN
        WARNED!... Really, this is important*/
        struct ParticleEmitterData
        {
            bool m_active;
            // Position of emitter
            XMFLOAT3 m_position;
            // Dimensions of emitter
            XMFLOAT2 m_dimensions;
            // Direction in which the emitter fires particles
            XMFLOAT4 m_direction;
            // Pressure with which particles are launched (arbitrary unit)
            float m_launchPressure;
            // How many particles are emitted per second
            float m_emissionRate;
            // How dense the particles are packed UNUSED TODOJB remove?
            float m_density;
            // The number of particles spawned along the local X axis
            int m_numParticlesX;
            // The number of particles spawned along the local Y axis
            int m_numParticlesY;
            // How wide and high the emission area is in radians
            XMFLOAT2 m_emissionAreaDimensions;
        };
        class FluidManager
        {
        public:
            // virtual void CreateFluid(int p_id) = 0;
            // virtual void CreateFluidParticles(int p_id, vector<XMFLOAT3>& p_positions, vector<XMFLOAT3>& p_velocities, vector<int>& p_indices) = 0;
            virtual void GetParticlePositions(int p_id, vector<XMFLOAT3>& o_positions) = 0;
            /**
            Creates a new particle system. Particle system will be a pressure emitter ONLY*/
            virtual void CreateParticleEmitter(int p_id, ParticleEmitterData p_data) = 0;
            /**
            Used to update the entire particle system*/
            virtual void SetParticleEmitterData(int p_id, ParticleEmitterData p_data) = 0;

            /**
            Gets all drains hit by a particle of the specified particle system*/
            virtual vector<int> GetDrainsHit(int p_id) = 0;
        };
    }
}