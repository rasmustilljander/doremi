#pragma once

#include <PhysX/PxPhysicsAPI.h>

using namespace physx;
namespace DoremiEngine
{
    namespace Physics
    {
        class PhysXSubmodule
        {
            public:
            PhysXSubmodule();
            virtual ~PhysXSubmodule();

            /**
            Initializes core physx components. Physx CANNOT be run without calling init
            Put in constructor?*/
            void InitPhysx();
            /**
            Creates a test scene. Subject to major refactoring, as it only exists to
            test PhysX and get a hang of how the API works. The risk is that this will
            be the entire PhysX master function however... But we're probably better than that*/
            void CreateTestScene();

            /**
            Adds a box to the global scene Positions and size of the box have to be provided
            Will very, VERY likely be refactored. */
            void AddBox(float p_posx, float p_posy, float p_posz, float p_dimx, float p_dimy, float p_dimz);

            /**
            Simulates a step of the simulation. In essence, updates everything*/
            void Update(float p_dt);

            private:
            PxDefaultAllocator m_allocator;
            PxDefaultErrorCallback m_errorCallback;

            PxFoundation* m_foundation;
            PxPhysics* m_physics;

            PxDefaultCpuDispatcher* m_dispatcher;
            // Scene in which objects stand. Probably should not be this global
            PxScene* m_scene;

            // Material for objects. Should NOT be this global. Only here for testing
            PxMaterial* m_material;

            PxReal m_stack = 10.0f;
        };
    }
}