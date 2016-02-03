#pragma once
#include <DirectXMath.h>

#include <PhysX/PxPhysicsAPI.h>
#include <FluidManager.hpp>
#include <vector>
#include <map>

#define PARTICLE_MAX_COUNT 200000 // TODOCONFIG
#define PARTICLE_MAX_MOTION_DISTANCE 100 // TODOCONFIG
using namespace DirectX;
using namespace physx;
using namespace std;
namespace DoremiEngine
{
    namespace Physics
    {
        struct ParticleEmitterData;
        struct InternalPhysicsUtils;
        /**
        Class which handles particle systems.
        It is responsible for emitting new particles*/
        class ParticleEmitter
        {
        public:
            // ParticleEmitter(
            //    XMFLOAT3 p_position,
            //    XMFLOAT4 p_direction,
            //    XMFLOAT2 p_dimensions,
            //    float p_launchPressure,
            //    float p_emissionRate,
            //    float p_density,
            //    XMFLOAT2 p_emissionAreaDimensions);
            ParticleEmitter(ParticleEmitterData p_data, InternalPhysicsUtils& p_utils);
            virtual ~ParticleEmitter();


            /**
            Gets all positions of particles associated with this
            particle system. Send a reference to an already defined
            vector of XMFLOATS as argument*/
            void GetPositions(vector<XMFLOAT3>& o_positions);

            /**
            Returns a vector with the index for all drain objects
            that were hit by a particle in the previous simulation
            step*/
            vector<int> GetDrainsHit();


            /**
            Sets all variables of the particle emitter*/
            void SetData(ParticleEmitterData p_data);

            /**
            Sets the position of the particle system*/
            void SetPosition(XMFLOAT3 p_position);
            /** Sets the direction in which the particle system
            emits particles*/
            void SetDirection(XMFLOAT4 p_direction);
            /**
            Sets whether gravity is active or not for particles
            in the system (true enables, false disables)*/
            void SetGravity(bool p_gravity);
            /**
            Update-loop for this particle system. Creates new
            particles (update as more stuff is added)*/
            void Update(float p_dt);
            /**
                UpdateLifetime of all active particles. Release particles that have lifetime < 0            
            */
            void UpdateParticleLifeTimeAndRemoveExpired(float p_dt);
            /**
                Sets the lifetime of the particles made after this call. Should be set before every particle call just to be safe that it is the correct timelength
            */
            void SetParticlesLifeTime(double p_time) { m_lifeTime = p_time; }

        private:
            ParticleEmitterData m_this;
            std::map<int, double> m_particlesLifeTime;
            double m_lifeTime;

            // The actual PhysX particle system
            PxParticleSystem* m_particleSystem;
            // Used to internally acces other physics thingies
            InternalPhysicsUtils& m_utils;
            // Float to keep track of when last particle wave was emitted
            float m_timeSinceLast;

            vector<int> m_drainsHit;

            // Temporary index variable to make sure things work
            int m_nextIndex;
        };
    }
}