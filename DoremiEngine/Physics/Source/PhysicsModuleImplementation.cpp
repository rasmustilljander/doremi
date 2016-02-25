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
            m_utils.m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_utils.m_foundation, PxTolerancesScale(), true);

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

        void PhysicsModuleImplementation::Shutdown()
        {
            m_collisionPairs.clear();
            m_triggerPairs.clear();
            m_leftCollisionPairs.clear();
        }

        void PhysicsModuleImplementation::Update(float p_dt)
        {
            // Start by clearing the list of collision pairs (WARNING potentially bad idea)
            m_collisionPairs.clear();
            m_triggerPairs.clear();
            m_leftCollisionPairs.clear();
            m_utils.m_fluidManager->Update(p_dt);
            m_utils.m_worldScene->simulate(p_dt);
            m_utils.m_rigidBodyManager->ClearRecentlyWakeStatusLists();
            m_utils.m_worldScene->fetchResults(true);
        }

        RigidBodyManager& PhysicsModuleImplementation::GetRigidBodyManager() { return *m_utils.m_rigidBodyManager; }
        PhysicsMaterialManager& PhysicsModuleImplementation::GetPhysicsMaterialManager() { return *m_utils.m_physicsMaterialManager; }
        CharacterControlManager& PhysicsModuleImplementation::GetCharacterControlManager() { return *m_utils.m_characterControlManager; }
        FluidManager& PhysicsModuleImplementation::GetFluidManager() { return *m_utils.m_fluidManager; }
        RayCastManager& PhysicsModuleImplementation::GetRayCastManager() { return *m_utils.m_rayCastManager; };

        vector<CollisionPair>& PhysicsModuleImplementation::GetCollisionPairs() { return m_collisionPairs; }
        vector<CollisionPair>& PhysicsModuleImplementation::GetTriggerPairs() { return m_triggerPairs; }
        vector<CollisionPair>& PhysicsModuleImplementation::GetLeftCollisionPairs() { return m_leftCollisionPairs; }

        vector<CollisionPair>& PhysicsModuleImplementation::GetLeftTriggerPairs() { return m_leftTriggerPairs; }

        // New fancier collision filter
        PxFilterFlags TestFilter2(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1,
                                  PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
        {


            if(filterData0.word2 == 8 || filterData1.word2 == 8)
            {
                int derp = 5;
            }

            // generate contacts for all that were not filtered above
            pairFlags = PxPairFlag::eCONTACT_DEFAULT;

            // controller vs. controller collisions TODOJB Verify fix
            bool kinematic0 = PxFilterObjectIsKinematic(attributes0);
            bool kinematic1 = PxFilterObjectIsKinematic(attributes1);

            if(kinematic0 && kinematic1)
            {
                pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
                return PxFilterFlag::eDEFAULT;
            }
            // Kill check (if 0 wants to ignore 1 or 1 wants to ignore 0)
            if((filterData0.word3 & filterData1.word0) || (filterData1.word3 & filterData0.word0))
            {
                return PxFilterFlag::eKILL;
            }
            // General filter if they match
            if((filterData0.word0 & filterData1.word1) || (filterData1.word0 & filterData0.word1))
            {
                pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
                return PxFilterFlag::eDEFAULT;
            }
            // Touch lost check
            if((filterData0.word0 & filterData1.word2) && (filterData1.word0 & filterData1.word2))
            {
                pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
                return PxFilterFlag::eDEFAULT;
            }


            // TODOJB fix so it uses generic words. NOT hard-coded
            // Filter out collisions with ignore-bodies REALLY UGLY TODOJB Improve
            // if(filterData0.word3 & 2) // been an ignore collision
            //{
            //    if(filterData0.word0 & 2) return PxFilterFlag::eKILL;
            //}
            // else if(filterData1.word3 & 2)
            //{
            //    if(filterData0.word0 & 2) return PxFilterFlag::eKILL;
            //}

            // Trigger collisions
            if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
            {
                pairFlags = PxPairFlag::eTRIGGER_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND;
                return PxFilterFlag::eDEFAULT;
            }

            return PxFilterFlags();
        }

        // Custom collision filter shader (OLD DEPRICATED CRAP. Still kept around for safety)
        // PxFilterFlags TestFilterOLD(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1,
        //                         PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
        //{
        //    // Rigid bodies collisions
        //    if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
        //    {
        //        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        //        pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
        //        pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
        //        pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
        //        return PxFilterFlag::eDEFAULT;
        //    }

        //    // controller vs. controller collisions
        //    bool kinematic0 = PxFilterObjectIsKinematic(attributes0);
        //    bool kinematic1 = PxFilterObjectIsKinematic(attributes1);

        //    if(kinematic0 && kinematic1)
        //    {
        //        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        //        return PxFilterFlag::eDEFAULT;
        //    }

        //    // generate contacts for all that were not filtered above
        //    pairFlags = PxPairFlag::eCONTACT_DEFAULT;

        //    // trigger the contact callback for pairs (A,B) where
        //    // the filtermask of A contains the ID of B and vice versa.
        //    if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
        //    {
        //        pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
        //    }

        //    return PxFilterFlag::eDEFAULT;
        //}

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
            sceneDesc.filterShader = TestFilter2;
            // Notify PhysX that we want callbacks to be called here
            sceneDesc.simulationEventCallback = this;

            // Create the scene
            m_utils.m_worldScene = m_utils.m_physics->createScene(sceneDesc);

            // Assign stuff to the global material (again, probably stupid to have this here)
            PxMaterial* groundMaterial = m_utils.m_physics->createMaterial(0.5, 0.5, 0.5);

            // Create the ground on which everything stands on. Possibly shouldn't here (member varialbe? Separate class?)
            PxPlane groundPlane = PxPlane(0, 1, 0, 10000); // change last digit for distance from origo

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
                // Check if this was a touch found collision callback
                if(cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
                {
                    /// Add first ID to collision pair
                    // Check if the first actor was a rigid body
                    if(m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairHeader.actors[0]) != m_utils.m_rigidBodyManager->GetIDsByBodies().end())
                    {
                        collisionPair.firstID = m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairHeader.actors[0])->second;
                    }
                    /*
                    Wasn't a rigid body. Assume it is a character controller (pretty risky but should be fine as long as no one decides
                    to assign a bunch of flags to particle systems. But that shouldn't happen. That's my country! Get out!)*/
                    else
                    {
                        unordered_map<PxController*, int> idsByControllers = m_utils.m_characterControlManager->GetIdsByControllers();
                        for(auto const& controller : idsByControllers)
                        {
                            if(controller.first->getActor() == pairHeader.actors[0])
                            {
                                collisionPair.firstID = controller.second;
                            }
                        }
                    }
                    /// Add second ID to collision pair
                    // Check if the first actor was a rigid body
                    if(m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairHeader.actors[1]) != m_utils.m_rigidBodyManager->GetIDsByBodies().end())
                    {
                        collisionPair.secondID = m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairHeader.actors[1])->second;
                    }
                    // Wasn't rigid body. Assume it is a character controller. Again, this should be fine
                    else
                    {
                        unordered_map<PxController*, int> idsByControllers = m_utils.m_characterControlManager->GetIdsByControllers();
                        for(auto const& controller : idsByControllers)
                        {
                            if(controller.first->getActor() == pairHeader.actors[1])
                            {
                                collisionPair.secondID = controller.second;
                            }
                        }
                    }
                    m_collisionPairs.push_back(collisionPair);
                }
                // Check if it was a touch lost collision (copy paste, yo!)
                else if(cp.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
                {
                    /// Add first ID to collision pair
                    // Check if the first actor was a rigid body
                    if(m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairHeader.actors[0]) != m_utils.m_rigidBodyManager->GetIDsByBodies().end())
                    {
                        collisionPair.firstID = m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairHeader.actors[0])->second;
                    }
                    /*
                    Wasn't a rigid body. Assume it is a character controller (pretty risky but should be fine as long as no one decides
                    to assign a bunch of flags to particle systems. But that shouldn't happen. That's my country! Get out!)*/
                    else
                    {
                        unordered_map<PxController*, int> idsByControllers = m_utils.m_characterControlManager->GetIdsByControllers();
                        for(auto const& controller : idsByControllers)
                        {
                            if(controller.first->getActor() == pairHeader.actors[0])
                            {
                                collisionPair.firstID = controller.second;
                            }
                        }
                    }
                    /// Add second ID to collision pair
                    // Check if the first actor was a rigid body
                    if(m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairHeader.actors[1]) != m_utils.m_rigidBodyManager->GetIDsByBodies().end())
                    {
                        collisionPair.secondID = m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairHeader.actors[1])->second;
                    }
                    // Wasn't rigid body. Assume it is a character controller. Again, this should be fine
                    else
                    {
                        unordered_map<PxController*, int> idsByControllers = m_utils.m_characterControlManager->GetIdsByControllers();
                        for(auto const& controller : idsByControllers)
                        {
                            if(controller.first->getActor() == pairHeader.actors[1])
                            {
                                collisionPair.secondID = controller.second;
                            }
                        }
                    }
                    m_leftCollisionPairs.push_back(collisionPair);
                }
            }
        }


        void PhysicsModuleImplementation::onTrigger(PxTriggerPair* pairs, PxU32 count)
        {
            for(size_t i = 0; i < count; i++)
            {
                const PxTriggerPair& cp = pairs[i];
                if(cp.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
                {
                    CollisionPair collisionPair;
                    // Get trigger ID
                    collisionPair.firstID = m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairs->triggerActor)->second;
                    // Determine whether other actor is controller or rigid body
                    if(m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairs->otherActor) == m_utils.m_rigidBodyManager->GetIDsByBodies().end())
                    {
                        /*
                        Was controller
                        Loop through all controllers to see which it is. This is silly, but because PxController and PxActor are two
                        Entierly different things, we have to do it this way. I think...*/
                        unordered_map<PxController*, int> idsByControllers = m_utils.m_characterControlManager->GetIdsByControllers();
                        for(auto const& controller : idsByControllers)
                        {
                            if(controller.first->getActor() == pairs->otherActor)
                            {
                                collisionPair.secondID = controller.second;
                            }
                        }
                    }
                    else
                    {
                        // Has to be a rigid body. Possibly dangerous to assume...
                        collisionPair.secondID = m_utils.m_rigidBodyManager->GetIDsByBodies().find(pairs->otherActor)->second;
                    }
                    m_triggerPairs.push_back(collisionPair);
                }
            }
        }

        void PhysicsModuleImplementation::onWake(PxActor** p_actors, PxU32 p_count)
        {
            m_utils.m_rigidBodyManager->SetRecentlyWokenObjects(p_actors, p_count);
        }

        void PhysicsModuleImplementation::onSleep(PxActor** p_actors, PxU32 p_count)
        {
            m_utils.m_rigidBodyManager->SetRecentlySleepingObjects(p_actors, p_count);
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