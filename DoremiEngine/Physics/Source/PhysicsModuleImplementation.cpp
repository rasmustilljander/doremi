#include <Internal/PhysicsModuleImplementation.hpp>


namespace DoremiEngine
{
    namespace Physics
    {
        PhysicsModuleImplementation::PhysicsModuleImplementation(const Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}
        PhysicsModuleImplementation::~PhysicsModuleImplementation() {}

        void PhysicsModuleImplementation::Startup()
        {
            // Start physX
            m_utils.m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_utils.m_allocator, m_utils.m_errorCallback);
            PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(m_utils.m_foundation);
            m_utils.m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_utils.m_foundation, PxTolerancesScale(), true, profileZoneManager);

            // Create world scene TODOJB create scene handler for this kind of job
            CreateWorldScene();

            // Create sub modules
            m_utils.m_rigidBodyManager = new RigidBodyManagerImpl(m_utils);
            m_utils.m_physicsMaterialManager = new PhysicsMaterialManagerImpl(m_utils);
            m_utils.m_characterControlManager = new CharacterControlManagerImpl(m_utils);
            m_utils.m_fluidManager = new FluidManagerImpl(m_utils);
            m_utils.m_rayCastManager = new RayCastManagerImpl(m_utils);

            // Make some other important thingies
            m_utils.m_characterControlManager->SetCallbackClass(this);

            if(m_utils.m_physics->getPvdConnectionManager() == NULL)
            {
                int failed = 1;
            }
            PxVisualDebuggerConnection* theConnection =
                PxVisualDebuggerExt::createConnection(m_utils.m_physics->getPvdConnectionManager(), "127.0.0.1", 5425, 100,
                                                      PxVisualDebuggerExt::getAllConnectionFlags());
            if(theConnection) theConnection->release();
        }

        void PhysicsModuleImplementation::Shutdown() {}

        void PhysicsModuleImplementation::Update(float p_dt)
        {
            // Start by clearing the list of collision pairs (WARNING potentially bad idea)
            m_collisionPairs.clear();
            m_triggerPairs.clear();
            m_utils.m_fluidManager->Update(p_dt);
            m_utils.m_worldScene->simulate(p_dt);
            m_utils.m_worldScene->fetchResults(true);
        }

        RigidBodyManager& PhysicsModuleImplementation::GetRigidBodyManager() { return *m_utils.m_rigidBodyManager; }
        PhysicsMaterialManager& PhysicsModuleImplementation::GetPhysicsMaterialManager() { return *m_utils.m_physicsMaterialManager; }
        CharacterControlManager& PhysicsModuleImplementation::GetCharacterControlManager() { return *m_utils.m_characterControlManager; }
        FluidManager& PhysicsModuleImplementation::GetFluidManager() { return *m_utils.m_fluidManager; }
        RayCastManager& PhysicsModuleImplementation::GetRayCastManager() { return *m_utils.m_rayCastManager; };

        vector<CollisionPair> PhysicsModuleImplementation::GetCollisionPairs() { return m_collisionPairs; }
        vector<CollisionPair> PhysicsModuleImplementation::GetTriggerPairs() { return m_triggerPairs; }

        // Custom collision filter shader
        PxFilterFlags TestFilter(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1,
                                 PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
        {
            // Rigid bodies collisions
            if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
            {
                pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
                pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
                pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
                pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
                return PxFilterFlag::eDEFAULT;
            }

            // controller vs. controller collisions
            bool kinematic0 = PxFilterObjectIsKinematic(attributes0);
            bool kinematic1 = PxFilterObjectIsKinematic(attributes1);

            if(kinematic0 && kinematic1)
            {
                pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
                return PxFilterFlag::eDEFAULT;
            }

            // generate contacts for all that were not filtered above
            pairFlags = PxPairFlag::eCONTACT_DEFAULT;

            // trigger the contact callback for pairs (A,B) where
            // the filtermask of A contains the ID of B and vice versa.
            if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
            {
                pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
            }

            return PxFilterFlag::eDEFAULT;
        }

        void PhysicsModuleImplementation::CreateWorldScene()
        {
            /// Create the scene
            // No idea what the "tolerances" is
            PxSceneDesc sceneDesc(m_utils.m_physics->getTolerancesScale());
            // Gravity sounds straight forward
            sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
            // Not too sure what this is. Probably related to how threads work
            m_utils.m_dispatcher = PxDefaultCpuDispatcherCreate(2);
            sceneDesc.cpuDispatcher = m_utils.m_dispatcher;
            // Some way of filtering collisions. Use default shaders since we cba to write our own
            sceneDesc.filterShader = TestFilter;
            // Notify PhysX that we want callbacks to be called here
            sceneDesc.simulationEventCallback = this;

            // Create the scene
            m_utils.m_worldScene = m_utils.m_physics->createScene(sceneDesc);

            // Assign stuff to the global material (again, probably stupid to have this here)
            PxMaterial* groundMaterial = m_utils.m_physics->createMaterial(0.5, 0.5, 0.5);

            // Create the ground on which everything stands on. Possibly shouldn't here (member varialbe? Separate class?)
            PxPlane groundPlane = PxPlane(0, 1, 0, 10); // change last digit for distance from origo

            PxRigidStatic* worldGround = PxCreatePlane(*m_utils.m_physics, groundPlane, *groundMaterial);
            // Add the ground plane to the scene. Apparently it's this easy
            m_utils.m_worldScene->addActor(*worldGround);
            // Not sure what this does... Desperate try maybe?
            m_utils.m_worldScene->setFlag(PxSceneFlag::eENABLE_KINEMATIC_PAIRS, true);

            // m_utils.m_worldScene->setFlag(PxSceneFlag::eENABLE_KINEMATIC_STATIC_PAIRS, true);

            /*
            And we now have a simple scene with gravity and the ground as a plane*/
        }

        void PhysicsModuleImplementation::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
        {
            m_utils.m_rigidBodyManager->GetIDsByBodies();
            for(size_t i = 0; i < nbPairs; i++)
            {
                const PxContactPair& cp = pairs[i];
                CollisionPair collisionPair;

                if(m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairHeader.actors[0]) != m_utils.m_rigidBodyManager->GetIDsByBodies().end())
                {
                    collisionPair.firstID = m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairHeader.actors[0])->second;
                }
                else
                {
                    // Probably hit a controller. TODOJB solve some time
                }

                if(m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairHeader.actors[1]) != m_utils.m_rigidBodyManager->GetIDsByBodies().end())
                {
                    collisionPair.secondID = m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairHeader.actors[1])->second;
                }
                else
                {
                    // Probably hit a controller. TODOJB solve some time
                }


                m_collisionPairs.push_back(collisionPair);
            }
        }


        void PhysicsModuleImplementation::onTrigger(PxTriggerPair* pairs, PxU32 count)
        {
            for(size_t i = 0; i < count; i++)
            {
                const PxTriggerPair& cp = pairs[i];
                CollisionPair collisionPair;
                collisionPair.firstID = m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairs->triggerActor)->second;
                collisionPair.secondID = m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairs->otherActor)->second;
                m_triggerPairs.push_back(collisionPair);
            }
        }

        void PhysicsModuleImplementation::onShapeHit(const PxControllerShapeHit& hit)
        {
            // TODOJB implement
        }
        PxUserControllerHitReport* derp;

        void PhysicsModuleImplementation::onControllerHit(const PxControllersHit& hit)
        {
            CollisionPair collisionPair;
            unordered_map<PxController*, int> idsByControllers = m_utils.m_characterControlManager->GetIdsByControllers();
            PxController* firstActor = hit.controller;
            PxController* secondActor = hit.other;
            int first = idsByControllers[firstActor];
            int second = idsByControllers[secondActor];

            collisionPair.firstID = m_utils.m_characterControlManager->GetIdsByControllers().find(hit.controller)->second;
            collisionPair.secondID = m_utils.m_characterControlManager->GetIdsByControllers().find(hit.other)->second;
            m_collisionPairs.push_back(collisionPair);
        }
    }
}

DoremiEngine::Physics::PhysicsModule* CreatePhysicsModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Physics::PhysicsModule* physics = new DoremiEngine::Physics::PhysicsModuleImplementation(p_sharedContext);
    return physics;
}