#pragma once
#include <PhysicsModule.hpp>
#include <Internal/RigidBodyManagerImpl.hpp>
#include <Internal/PhysicsMaterialManagerImpl.hpp>
#include <Internal/PhysicsModuleImplementation.hpp>
#include <Internal/CharacterControlManagerImpl.hpp>
#include <Internal/FluidManagerImpl.hpp>
#include <Internal/RayCastManagerImpl.hpp>

#include <PhysX/PxPhysicsAPI.h>
#include <PhysX/pvd/PxVisualDebugger.h>
#include <PhysX/physxvisualdebuggersdk/PvdConnection.h>

using namespace physx;
namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }

    namespace Physics
    {
        // Internal struct to be used for submodules to communicate
        struct InternalPhysicsUtils
        {
            ~InternalPhysicsUtils()
            {
                delete m_rigidBodyManager;
                delete m_physicsMaterialManager;
                delete m_characterControlManager;
                delete m_fluidManager;
                delete m_rayCastManager;
                m_worldScene->release();
                m_physics->release();
                m_dispatcher->release();
                m_foundation->release();
            }

            // Sub modules managers thingies
            RigidBodyManagerImpl* m_rigidBodyManager;
            PhysicsMaterialManagerImpl* m_physicsMaterialManager;
            CharacterControlManagerImpl* m_characterControlManager;
            FluidManagerImpl* m_fluidManager;
            RayCastManagerImpl* m_rayCastManager;

            // PhysX stuff
            PxDefaultAllocator m_allocator;
            PxDefaultErrorCallback m_errorCallback;
            PxPhysics* m_physics;
            PxDefaultCpuDispatcher* m_dispatcher;
            PxFoundation* m_foundation;

            // The basic world. TODOJB add to some sort of scene manager?
            PxScene* m_worldScene;
        };

        class PhysicsModuleImplementation : public PhysicsModule, public PxSimulationEventCallback, public PxUserControllerHitReport
        {
        public:
            PhysicsModuleImplementation(const Core::SharedContext& p_sharedContext);
            virtual ~PhysicsModuleImplementation();

            /**
            Starts the physics module (inits PhysX so far)*/
            void Startup() override;
            void Shutdown() override;
            /**
            Does nothing so far TODOJB update as necessary*/
            void Update(float p_dt) override;

            /**
            Get methods for sub modules*/
            RigidBodyManager& GetRigidBodyManager() override;
            PhysicsMaterialManager& GetPhysicsMaterialManager() override;
            CharacterControlManager& GetCharacterControlManager() override;
            FluidManager& GetFluidManager() override;
            RayCastManager& GetRayCastManager() override;

            vector<CollisionPair> GetCollisionPairs() override;
            vector<CollisionPair> GetTriggerPairs() override;
            vector<CollisionPair> GetLeftCollisionPairs() override;
            vector<CollisionPair> GetLeftTriggerPairs() override;

        private:
            // Creates the world as a scene. TODOJB create SceneManager somehow
            void CreateWorldScene();

            /// Implements PxSimulationEventCallback
            // Called when contact between two bodies (i think)
            virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
            // NOT USED
            virtual void onTrigger(PxTriggerPair* pairs, PxU32 count);
            // NOT USED
            virtual void onConstraintBreak(PxConstraintInfo*, PxU32) {}
            // NOT USED
            virtual void onWake(PxActor**, PxU32);
            // NOT USED
            virtual void onSleep(PxActor**, PxU32);
            /// End PxSimulationEventCallback

            /// Implements PxUserCharacterControlCallback
            // Called when controller hits a body
            virtual void onShapeHit(const PxControllerShapeHit& hit);
            // Called when controller hits another controller
            virtual void onControllerHit(const PxControllersHit& hit);
            // NOT USED
            virtual void onObstacleHit(const PxControllerObstacleHit& hit) {}

            // Has shared context for some reason. Shouldn't be here
            const Core::SharedContext& m_sharedContext;

            // Struct for shared physics thingies within the module
            InternalPhysicsUtils m_utils;

            // Vector over IDs of bodies that collided
            vector<CollisionPair> m_collisionPairs;
            vector<CollisionPair> m_triggerPairs;
            vector<CollisionPair> m_leftCollisionPairs;
            vector<CollisionPair> m_leftTriggerPairs;
        };
    }
}