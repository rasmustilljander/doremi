#include <PhysxSubmodule.Hpp>

namespace DoremiEngine
{
    namespace Physics
    {
        PhysXSubmodule::PhysXSubmodule() {}
        PhysXSubmodule::~PhysXSubmodule() {}

        void PhysXSubmodule::InitPhysx()
        {
            m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_allocator, m_errorCallback);
            PxProfileZoneManager* profileZoneManager = &PxProfileZoneManager::createProfileZoneManager(m_foundation);
            m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, PxTolerancesScale(), true, profileZoneManager);
        }

        void PhysXSubmodule::CreateTestScene()
        {
            /// Create the scene
            // No idea what the "tolerances" is
            PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
            // Gravity sounds straight forward
            sceneDesc.gravity = PxVec3(0, -9.8, 0);
            // Not too sure what this is. Probably related to how threads work
            m_dispatcher = PxDefaultCpuDispatcherCreate(2);
            sceneDesc.cpuDispatcher = m_dispatcher;
            // Some way of filtering collisions. Use default shaders since we cba to write our own
            sceneDesc.filterShader = PxDefaultSimulationFilterShader;
            m_scene = m_physics->createScene(sceneDesc);

            // Assign stuff to the global material (again, probably stupid to have this here)
            m_material = m_physics->createMaterial(0.5, 0.5, 0.5);

            // Create the ground on which everything stands on. Possibly shouldn't here (member varialbe? Separate class?)
            PxRigidStatic* groundPlane = PxCreatePlane(*m_physics, PxPlane(0, 1, 0, 0), *m_material);

            // Add the ground plane to the scene. Apparently it's this easy
            m_scene->addActor(*groundPlane);

            /*
            And we now have a simple scene with gravity and the ground as a plane*/
        }

        void PhysXSubmodule::AddBox(float p_posx, float p_posy, float p_posz, float p_dimx, float p_dimy, float p_dimz)
        {
            // Create a world matrix (only translation) i think
            PxTransform transform = PxTransform(PxVec3(p_posx, p_posy, p_posz));
            // Creates the physics object shape thingy, which collides with stuff. Shapes are just objects. I think
            PxShape* shape = m_physics->createShape(PxBoxGeometry(p_dimx, p_dimy, p_dimz), *m_material);
            // Creates the actual body.
            PxRigidDynamic* body = m_physics->createRigidDynamic(transform);

            // body->attachShape()
        }
    }
}