#include <PhysicsModuleImplementation.hpp>

#include <Bullet/btBulletDynamicsCommon.h>
namespace DoremiEngine
{
    namespace Physics
    {
        PhysicsModuleImplementation::PhysicsModuleImplementation()
        {
        }
        PhysicsModuleImplementation::~PhysicsModuleImplementation()
        {
        }

        void PhysicsModuleImplementation::Startup()
        {
        }

        void PhysicsModuleImplementation::SetWorkingDirectory(const std::string& p_workingDirectory)
        {
        }

        void PhysicsModuleImplementation::Shutdown()
        {
        }

        // Only an example method to demonstrate how the engine is used
        float PhysicsModuleImplementation::ExampleMethod(const float& posx)
        {
            return 1;
        }

        void PhysicsModuleImplementation::ExampleMethodAltersData(float* p_posX, float* p_posY)
        {
            *p_posX = 5;
            *p_posY = *p_posX * 2;
        }
    }
}

DoremiEngine::Physics::PhysicsModule* CreatePhysicsModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Physics::PhysicsModule* physics = new DoremiEngine::Physics::PhysicsModuleImplementation();
    return physics;
}