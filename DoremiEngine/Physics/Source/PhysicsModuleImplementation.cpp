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

            // Create sub modules
            m_utils.m_rigidBodyManager = new RigidBodyManagerImpl(m_utils);
            m_utils.m_physicsMaterialManager = new PhysicsMaterialManagerImpl(m_utils);

            // Create world scene TODOJB create scene handler for this kind of job
            CreateWorldScene();
        }

        void PhysicsModuleImplementation::Shutdown() {}

        void PhysicsModuleImplementation::Update(float p_dt)
        {
            m_utils.m_worldScene->simulate(p_dt);
            m_utils.m_worldScene->fetchResults(true);
        }

        RigidBodyManager& PhysicsModuleImplementation::GetRigidBodyManager() { return *m_utils.m_rigidBodyManager; }
        PhysicsMaterialManager& PhysicsModuleImplementation::GetPhysicsMaterialManager() { return *m_utils.m_physicsMaterialManager; }

        // Only an example method to demonstrate how the engine is used
        float PhysicsModuleImplementation::ExampleMethod(const float& posx) { return 1; }

        void PhysicsModuleImplementation::ExampleMethodAltersData(float* p_posX, float* p_posY)
        {
            *p_posX = 5;
            *p_posY = *p_posX * 2;
        }

        void PhysicsModuleImplementation::CreateWorldScene()
        {
            /// Create the scene
            // No idea what the "tolerances" is
            PxSceneDesc sceneDesc(m_utils.m_physics->getTolerancesScale());
            // Gravity sounds straight forward
            sceneDesc.gravity = PxVec3(0, -9.8, 0);
            // Not too sure what this is. Probably related to how threads work
            m_utils.m_dispatcher = PxDefaultCpuDispatcherCreate(2);
            sceneDesc.cpuDispatcher = m_utils.m_dispatcher;
            // Some way of filtering collisions. Use default shaders since we cba to write our own
            sceneDesc.filterShader = PxDefaultSimulationFilterShader;
            m_utils.m_worldScene = m_utils.m_physics->createScene(sceneDesc);

            // Assign stuff to the global material (again, probably stupid to have this here)
            PxMaterial* groundMaterial = m_utils.m_physics->createMaterial(0.5, 0.5, 0.5);

            // Create the ground on which everything stands on. Possibly shouldn't here (member varialbe? Separate class?)
            PxPlane groundPlane = PxPlane(0, 1, 0, 10); // change last 0 for distance from origo
            PxRigidStatic* worldGround = PxCreatePlane(*m_utils.m_physics, groundPlane, *groundMaterial);
            // Add the ground plane to the scene. Apparently it's this easy
            m_utils.m_worldScene->addActor(*worldGround);

            /*
            And we now have a simple scene with gravity and the ground as a plane*/
        }

        void PhysicsModuleImplementation::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {}

        void PhysicsModuleImplementation::onTrigger(PxTriggerPair* pairs, PxU32 count) {}
    }
}

DoremiEngine::Physics::PhysicsModule* CreatePhysicsModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Physics::PhysicsModule* physics = new DoremiEngine::Physics::PhysicsModuleImplementation(p_sharedContext);
    return physics;
}