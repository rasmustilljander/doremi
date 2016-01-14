#pragma once
#include <DirectXMath.h>

#include <PhysX/PxPhysicsAPI.h>
#include <FluidManager.hpp>
#include <vector>

#define PARTICLE_MAX_COUNT 10000 // TODOJB specify this some other way?
#define PARTICLE_MAX_MOTION_DISTANCE 100
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

            private:
            ParticleEmitterData m_this;
            //// Position of emitter
            // XMFLOAT3 m_position;
            //// Dimensions of emitter
            // XMFLOAT2 m_dimensions; // UNUSED ATM
            //// Direction in which the emitter fires particles
            // XMFLOAT4 m_direction;
            //// Pressure with which particles are launched (arbitrary unit)
            // float m_launchPressure;
            //// How many particles are emitted per second
            // float m_emissionRate;
            //// How dense the particles are packed
            // float m_density;
            //// How wide and high the emission area is in radians
            // XMFLOAT2 m_emissionAreaDimensions;

            // The actual PhysX particle system
            PxParticleSystem* m_particleSystem;
            // Used to internally acces other physics thingies
            InternalPhysicsUtils& m_utils;
            // Float to keep track of when last particle wave was emitted
            float m_timeSinceLast;

            // Temporary index variable to make sure things work
            int m_nextIndex;
        };
    }
}