// This class
#include <Internal/ParticleClasses/ParticleEmitter.hpp>
//#include <FluidManager.hpp> // Included for the sake of particle data. Kinda silly, really
#include <Internal/PhysicsModuleImplementation.hpp>

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
            m_utils.m_worldScene->addActor(*m_particleSystem);
            // Might be necessary to set flag to collide with dynamics
            // m_particleSystem->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_WITH_DYNAMIC_ACTORS, true);
            m_particleSystem->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true); // Should not be here
        }
        ParticleEmitter::~ParticleEmitter() {}

        void ParticleEmitter::SetPosition(XMFLOAT3 p_position) { m_this.m_position = p_position; }
        void ParticleEmitter::SetDirection(XMFLOAT4 p_direction) { m_this.m_direction = p_direction; }
        void ParticleEmitter::SetGravity(bool p_gravity) { m_particleSystem->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, p_gravity); }

        void ParticleEmitter::GetPositions(vector<XMFLOAT3>& o_positions)
        {
            // This is a bit harder than it really ought to be...
            PxParticleReadData* readData = m_particleSystem->lockParticleReadData();
            PxStrideIterator<const PxVec3> positions = readData->positionBuffer;
            PxStrideIterator<const PxVec3> velocities = readData->velocityBuffer;
            vector<XMFLOAT3> velocitiesVector;
            int numParticles = readData->validParticleRange;
            for(size_t i = 0; i < numParticles; i++)
            {
                o_positions.push_back(XMFLOAT3(positions[i].x, positions[i].y, positions[i].z));
            }
            readData->unlock();
        }

        void ParticleEmitter::Update(float p_dt)
        {

            // Update time since last particle wave was spawned
            m_timeSinceLast += p_dt;
            if(m_timeSinceLast > m_this.m_emissionRate)
            {
                m_timeSinceLast = 0;
                vector<XMFLOAT3> velocities;
                vector<XMFLOAT3> positions;
                vector<int> indices;
                /// Time for more particles!
                /// These particles will be spawned in a sort of grid (atm)
                for(int x = -m_this.m_density; x < m_this.m_density * 2 - 1; x++)
                {
                    for(int y = -m_this.m_density; y < m_this.m_density * 2; y++)
                    {
                        // Calculate angles in local space
                        float xAngle = (x / m_this.m_density) * m_this.m_emissionAreaDimensions.x;
                        float yAngle = (y / m_this.m_density) * m_this.m_emissionAreaDimensions.y;
                        // Define standard target in local space
                        XMVECTOR particleVelocityVec = XMLoadFloat3(&XMFLOAT3(0, 0, 1));
                        XMMATRIX rotMatLocal = XMMatrixRotationRollPitchYaw(xAngle, yAngle, 0);
                        particleVelocityVec = XMVector3Transform(particleVelocityVec, rotMatLocal);
                        // Move velocity vector into world space
                        XMMATRIX rotMatWorld = XMMatrixRotationQuaternion(XMLoadFloat4(&m_this.m_direction));
                        particleVelocityVec = XMVector3Transform(particleVelocityVec, rotMatWorld);
                        // Multiply with pressure
                        particleVelocityVec *= m_this.m_launchPressure;
                        // Store in vector
                        XMFLOAT3 velocity;
                        XMStoreFloat3(&velocity, particleVelocityVec);
                        velocities.push_back(velocity);

                        // Add position (only emitts from the center of the emitter atm
                        positions.push_back(m_this.m_position);

                        // Add index (silly way just to make it work atm)
                        indices.push_back(m_nextIndex);
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
        }
    }
}