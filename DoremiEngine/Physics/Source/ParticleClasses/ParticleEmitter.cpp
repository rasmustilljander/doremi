// This class
#include <Internal/ParticleClasses/ParticleEmitter.hpp>
//#include <FluidManager.hpp> // Included for the sake of particle data. Kinda silly, really
#include <Internal/PhysicsModuleImplementation.hpp>

// 3rd parties

#include <iostream>
using namespace std;

using namespace DirectX;
using namespace physx;
namespace DoremiEngine
{
    namespace Physics
    {
        ParticleEmitter::ParticleEmitter(ParticleEmitterData p_data, InternalPhysicsUtils& p_utils)
            : m_this(p_data), m_utils(p_utils), m_timeSinceLast(0)
        {
            m_nextIndex = 0;
            m_particleSystem = m_utils.m_physics->createParticleSystem(PARTICLE_MAX_COUNT);
            m_particleSystem->setMaxMotionDistance(PARTICLE_MAX_MOTION_DISTANCE);
            m_particleSystem->setParticleReadDataFlag(PxParticleReadDataFlag::eVELOCITY_BUFFER, true);
            m_particleSystem->setContactOffset(m_this.m_size);
            m_utils.m_worldScene->addActor(*m_particleSystem);
            m_particleSystem->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true); // Should not be here
        }
        ParticleEmitter::~ParticleEmitter() {}

        void ParticleEmitter::SetPosition(XMFLOAT3 p_position) { m_this.m_position = p_position; }
        void ParticleEmitter::SetDirection(XMFLOAT4 p_direction) { m_this.m_direction = p_direction; }
        void ParticleEmitter::SetGravity(bool p_gravity) { m_particleSystem->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, p_gravity); }

        void ParticleEmitter::GetPositions(vector<XMFLOAT3>& o_positions)
        {
            // This is a bit harder than it really ought to be...
            // Important to lock data
            LockParticleData();
            // Get the positions
            PxStrideIterator<const PxVec3> positions = m_readData->positionBuffer;
            // And the flags (so we know that the particles are valid)
            PxStrideIterator<const PxParticleFlags> flags = m_readData->flagsBuffer;

            uint32_t numParticles = m_readData->validParticleRange;
            for(uint32_t i = 0; i < numParticles; i++)
            {
                // Check valid flag
                if(flags[i] & PxParticleFlag::eVALID)
                {
                    // Save the particle position
                    o_positions.push_back(XMFLOAT3(positions[i].x, positions[i].y, positions[i].z));
                }
            }
            // Unlock once we're done
            UnlockParticleData();
        }

        vector<int> ParticleEmitter::GetDrainsHit() { return m_drainsHit; }

        const vector<XMFLOAT3>& ParticleEmitter::GetRemovedParticlesPositions() { return m_removedParticlesPositions; }

        void ParticleEmitter::SetData(ParticleEmitterData p_data) { m_this = p_data; }

        void ParticleEmitter::LockParticleData() { m_readData = m_particleSystem->lockParticleReadData(); }

        void ParticleEmitter::UnlockParticleData() { m_readData->unlock(); }

        void ParticleEmitter::UpdateParticleLifeTimeAndRemoveExpired(float p_dt)
        {
            std::vector<PxU32> t_indicesToRemove;
            // Get particle indexData
            PX_ASSERT(t_particleData);
            // Kolla om vi har några particlar som är valid annars onödigt att ens börja uppdatera
            if(m_readData->validParticleRange > 0)
            {
                int length = m_readData->validParticleRange;

                for(PxU32 i = 0; i <= (m_readData->validParticleRange - 1) >> 5; i++)
                {

                    for(PxU32 j = m_readData->validParticleBitmap[i]; j; j &= j - 1)
                    {
                        // TODOXX super unsafe. I am not sure what i am doing here. I was following a tutorial and a function they used was missing.
                        // This seems to be what they did essentially but WARNING!!
                        // Get index from physx. you have to use bitoperations from their bittable for indices. Its pretty complicated and Im not 100%
                        // sure this code is safe
                        unsigned long b;
                        _BitScanForward(&b, j);
                        PxU32 t_index = (i << 5 | (PxU32)b);
                        // Update particle lifetime
                        m_particlesLifeTime[t_index] -= p_dt;
                        // CHeck if particle is still alive
                        if((m_particlesLifeTime[t_index]) < 0.0)
                        {
                            m_particlesLifeTime[t_index] = 0;
                            t_indicesToRemove.push_back(t_index);
                        }
                    }
                }
            }
            else
            {
                // Do nothing
            }
            // Release the data we have been looking at
            m_readData->unlock();
            // Needs to not crash
            if(t_indicesToRemove.size() > 0)
            {
                // Need Stride for releaseparticle.
                PxStrideIterator<const PxU32> t_indexData(&t_indicesToRemove[0]);
                // Now release the particles that we want to remove
                m_particleSystem->releaseParticles(static_cast<PxU32>(t_indicesToRemove.size()), t_indexData);
                // TODOLH om vi skaffar indexPool kör m_indexpool->freeIndices(indices.size(), indexData);
            }
        }

        void ParticleEmitter::RemoveDrainCollidedParticles(float p_dt)
        {
            // This is a bit harder than it really ought to be...
            PxStrideIterator<const PxVec3> positions = m_readData->positionBuffer;
            PxStrideIterator<const PxVec3> velocities = m_readData->velocityBuffer;
            PxStrideIterator<const PxParticleFlags> flags = m_readData->flagsBuffer;
            vector<XMFLOAT3> velocitiesVector;

            vector<int> indicesOfParticlesToBeReleased;

            uint32_t numParticles = m_readData->validParticleRange;
            for(uint32_t i = 0; i < numParticles; i++)
            {
                // Check if particles are supposed to be removed
                XMFLOAT3 position = XMFLOAT3(positions[i].x, positions[i].y, positions[i].z);
                XMFLOAT3 velocity = XMFLOAT3(velocities[i].x, velocities[i].y, velocities[i].z);
                XMVECTOR velVec = XMLoadFloat3(&velocity);
                XMVECTOR posVec = XMLoadFloat3(&position);
                // posVec -= velVec; this should be necessary but somehow it isn't. Possibly related to m_this.size?
                velVec = XMVector3Normalize(velVec);
                XMStoreFloat3(&velocity, velVec); // TODOJB Remove normalization once it's fixed in CastRay
                XMStoreFloat3(&position, posVec);
                if(flags[i] & (PxParticleFlag::eCOLLISION_WITH_DRAIN)) // | PxParticleFlag::eVALID))
                {
                    /// Particle should be removed
                    // Add index to release list
                    indicesOfParticlesToBeReleased.push_back(i);
                    // m_drainsHit.push_back(m_utils.m_rayCastManager->CastRay(position, velocity, 5)); // Zero might turn up buggy
                    // Use internal method since we want to know which type of body we hit
                    int flags;
                    int id = m_utils.m_rayCastManager->CastSweep(position, velocity, m_this.m_size, 100);
                    m_drainsHit.push_back(id);
                    // We don't want to notify the game when particles hit kinematic objects
                    // Outcommented since it doesn't work. TODOJB fix
                    /*if(!(flags == 0 || flags == 1))
                    {*/
                    // Add the position to a the list of removed particles positions
                    m_removedParticlesPositions.push_back(position);
                    //}
                }
            }
            if(indicesOfParticlesToBeReleased.size() != 0)
            {
                PxStrideIterator<const PxU32> inicesPX(reinterpret_cast<PxU32*>(&indicesOfParticlesToBeReleased[0]));
                m_particleSystem->releaseParticles(indicesOfParticlesToBeReleased.size(), inicesPX);
            }
        }

        void ParticleEmitter::UpdateParticleEmission(float p_dt)
        {
            // Factor to cause some more spray
            float sprayFactor = 0.1; // TODOCONFIG
            if(m_this.m_active)
            {
                // Update time since last particle wave was spawned
                m_timeSinceLast += p_dt;
                if(m_timeSinceLast > m_this.m_emissionRate)
                {
                    m_timeSinceLast = 0;
                    vector<XMFLOAT3> velocities;
                    vector<XMFLOAT3> positions;
                    vector<int> indices;
                    // TODOJB fix even particle numbers
                    int halfParticlesx = m_this.m_numParticlesX / 2;
                    int halfParticlesy = m_this.m_numParticlesY / 2;

                    /// Calculate new particles
                    /**
                    We want a grid generated that is of dimension numParticlesX * numparticlexY.
                    */
                    // Calculate matrix to rotate velocity to world space
                    XMMATRIX rotMatWorld = XMMatrixRotationQuaternion(XMLoadFloat4(&m_this.m_direction));
                    for(int x = -halfParticlesx; x < halfParticlesx + 1; x++) //+1 since we want at least one particle
                    {
                        // Calculate angle in local space
                        float xAngle = ((float)x / (float)m_this.m_numParticlesX) * m_this.m_emissionAreaDimensions.x;
                        // Add some randomness to make them spray more
                        uniform_real_distribution<float> distribution(-sprayFactor, sprayFactor);
                        xAngle += distribution(m_randomGenerator);
                        for(int y = -halfParticlesy; y < halfParticlesy + 1; y++)
                        {
                            // Calculate angle in local space
                            float yAngle = ((float)y / (float)m_this.m_numParticlesY) * m_this.m_emissionAreaDimensions.y;
                            yAngle += distribution(m_randomGenerator);
                            // Calculate velocity vector in local space
                            XMVECTOR particleVelocityVec = XMLoadFloat3(&XMFLOAT3(0, 0, 1));
                            XMMATRIX rotMatLocal = XMMatrixRotationRollPitchYaw(yAngle, xAngle, 0);
                            particleVelocityVec = XMVector3Transform(particleVelocityVec, rotMatLocal);
                            // Move velocity vector into world space
                            particleVelocityVec = XMVector3Transform(particleVelocityVec, rotMatWorld);
                            // Multiply with pressure
                            particleVelocityVec *= m_this.m_launchPressure;
                            // Store in vector
                            XMFLOAT3 velocity;
                            XMStoreFloat3(&velocity, particleVelocityVec);
                            velocities.push_back(velocity);
                            // Add position (only emitts from the center of the emitter atm
                            float launchOffset = 1.8f; // Intended hard coded value. TODOJB Set as parameter based on movement speed?
                            XMVECTOR positionVec = XMLoadFloat3(&m_this.m_position);
                            positionVec += launchOffset * XMVector3Normalize(particleVelocityVec);
                            XMFLOAT3 position;
                            XMStoreFloat3(&position, positionVec);
                            positions.push_back(position);

                            // Add index (silly way just to make it work atm)
                            indices.push_back(m_nextIndex);
                            // Set the lifetime of this particle TODOLH maybe add support for particles without lifetime. Some kind of bool. Hopefully
                            // we dont have to do this. Seems hard at first glance
                            m_particlesLifeTime[m_nextIndex] = m_lifeTime;
                            m_nextIndex++;
                        }
                    }

                    if(positions.size() > 0 && !(m_nextIndex > PARTICLE_MAX_COUNT)) // no point doing things if there's no new particles
                    {
                        // Cast into PhysX datatypes
                        PxVec3* positionsPX = reinterpret_cast<PxVec3*>(&positions[0]);
                        PxVec3* velocitiesPX = reinterpret_cast<PxVec3*>(&velocities[0]);
                        PxU32* indicesPX = reinterpret_cast<PxU32*>(&indices[0]);


                        // Create the particles
                        PxParticleCreationData newParticlesData;
                        newParticlesData.numParticles = positions.size();
                        newParticlesData.positionBuffer = PxStrideIterator<const PxVec3>(positionsPX);
                        newParticlesData.velocityBuffer = PxStrideIterator<const PxVec3>(velocitiesPX);
                        newParticlesData.indexBuffer = PxStrideIterator<const PxU32>(indicesPX);
                        m_particleSystem->createParticles(newParticlesData);
                    }
                    else
                    {
                        // No new particles. Do nothing
                    }
                }
                else
                {
                    // Not time for new particles
                }
            }
            else
            {
                // Particle system is not active
            }
        }

        void ParticleEmitter::Update(float p_dt)
        {
            m_removedParticlesPositions.clear();
            m_drainsHit.clear();
            // Lock the read data
            LockParticleData();
            // Remove old (aged) particles
            UpdateParticleLifeTimeAndRemoveExpired(p_dt);
            // Spray new particles
            UpdateParticleEmission(p_dt);
            // Remove particles that have collided with a drain
            RemoveDrainCollidedParticles(p_dt);
            // Unlock particle data
            UnlockParticleData();
        }
    }
}