#pragma once
// This calss

#include <Internal/ParticleClasses/ExperimentalParticleEmitter.hpp>

#include <FluidManager.hpp>
#include <unordered_map>
#include <PhysX/PxPhysicsAPI.h>
using namespace std;
using namespace physx;
namespace DoremiEngine
{
    namespace Physics
    {
        struct InternalPhysicsUtils;
        class FluidManagerImpl : public FluidManager
        {
            public:
            FluidManagerImpl(InternalPhysicsUtils& p_utils);
            virtual ~FluidManagerImpl();

            void CreateFluid(int p_id);
            void CreateFluidParticles(int p_id, vector<XMFLOAT3>& p_positions, vector<XMFLOAT3>& p_velocities, vector<int>& p_indices);
            vector<XMFLOAT3> GetParticlePositions(int p_id);


            private:
            InternalPhysicsUtils& m_utils;
            unordered_map<int, PxParticleFluid*> m_fluids;
        };
    }
}