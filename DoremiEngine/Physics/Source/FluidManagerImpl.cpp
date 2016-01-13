#include <Internal/FluidManagerImpl.hpp>
#include <Internal/PhysicsModuleImplementation.hpp>
namespace DoremiEngine
{
    namespace Physics
    {
        FluidManagerImpl::FluidManagerImpl(InternalPhysicsUtils& p_utils) : m_utils(p_utils) {}
        FluidManagerImpl::~FluidManagerImpl() {}

        void FluidManagerImpl::CreateFluid(int p_id)
        {
            float maxParticles = 100;
            bool perParticleRestOffset = false;

            // Create the particle systen
            PxParticleSystem* particleSystem = m_utils.m_physics->createParticleSystem(maxParticles, perParticleRestOffset);

            // Add particle system to the world
            if(particleSystem)
            {
                m_utils.m_worldScene->addActor(*particleSystem);
            }
        }
        void FluidManagerImpl::CreateFluidParticles(int p_id, vector<XMFLOAT3>& p_positions, vector<XMFLOAT3>& p_velocities, vector<int32_t>& p_indices)
        {
            // Cast XMFLOAT3s into something that PhysX likes. Possible since the data of the types are identical. Thanks, Bolt!
            PxVec3* positions = reinterpret_cast<PxVec3*>(&p_positions[0]);
            PxVec3* velocities = reinterpret_cast<PxVec3*>(&p_velocities[0]);
            PxU32* indices = reinterpret_cast<PxU32*>(&p_indices[0]);
            // Create the data for the particles
            PxParticleCreationData particleData;
            particleData.positionBuffer = PxStrideIterator<const PxVec3>(positions);
            particleData.velocityBuffer = PxStrideIterator<const PxVec3>(velocities);
            particleData.indexBuffer = PxStrideIterator<const PxU32>(indices);
            // Check that the data is valid
            if(particleData.isValid())
            {
                // Create the particles
                m_fluids[p_id]->createParticles(particleData);
            }
            else
            {
                // Shit went wrong! Handle somehow?
            }
        }

        vector<XMFLOAT3> FluidManagerImpl::GetParticlePositions(int p_id)
        {
            PxParticleFluidReadData* readData = m_fluids[p_id]->lockParticleFluidReadData();
            return vector<XMFLOAT3>();
        }
    }
}