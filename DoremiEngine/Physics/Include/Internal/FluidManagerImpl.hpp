#pragma once
// This calss
#include <FluidManager.hpp>
#include <unordered_map>
#include <PhysX/PxPhysicsAPI.h>

using namespace std;
using namespace physx;
namespace DoremiEngine
{
    namespace Physics
    {
        class ParticleEmitter;
        struct InternalPhysicsUtils;
        class FluidManagerImpl : public FluidManager
        {
            public:
            FluidManagerImpl(InternalPhysicsUtils& p_utils);
            virtual ~FluidManagerImpl();

            void CreateParticleEmitter(int p_id, ParticleEmitterData p_data) override;
            void GetParticlePositions(int p_id, vector<XMFLOAT3>& o_positions) override;
            // void CreateFluid(int p_id) override;
            // void CreateFluidParticles(int p_id, vector<XMFLOAT3>& p_positions, vector<XMFLOAT3>& p_velocities, vector<int>& p_indices) override;

            /// Internal methods
            // Updates all particle emitters
            void Update(float p_dt);

            private:
            InternalPhysicsUtils& m_utils;
            unordered_map<int, ParticleEmitter*> m_emitters;
        };
    }
}