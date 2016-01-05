#pragma once
#include <PhysicsModule.hpp>
#include <Internal/RigidBodyManagerImpl.hpp>
#include <Internal/PhysicsMaterialManagerImpl.hpp>
#include <Internal/PhysicsModuleImplementation.hpp>

#include <PhysX/PxPhysicsAPI.h>

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
            // Sub modules managers thingies
            RigidBodyManagerImpl* m_rigidBodyManager;
            PhysicsMaterialManagerImpl* m_physicsMaterialManager;

            // PhysX stuff
            PxDefaultAllocator m_allocator;
            PxDefaultErrorCallback m_errorCallback;

            PxFoundation* m_foundation;
            PxPhysics* m_physics;

            PxDefaultCpuDispatcher* m_dispatcher;

            // The basic world. TODOJB add to some sort of scene manager?
            PxScene* m_worldScene;
        };
        class PhysicsModuleImplementation : public PhysicsModule, public PxSimulationEventCallback
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

            /**
            Example methods used to display how the engine can be used.
            TODOJB remove*/
            float ExampleMethod(const float& posx) override;
            void ExampleMethodAltersData(float* p_posX, float* p_posY);

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
            virtual void onWake(PxActor**, PxU32) {}
            // NOT USED
            virtual void onSleep(PxActor**, PxU32) {}
            /// End PxSimulationEventCallback

            // Has shared context for some reason. Shouldn't be here
            const Core::SharedContext& m_sharedContext;

            // Struct for shared physics thingies within the module
            InternalPhysicsUtils m_utils;
        };
    }
}