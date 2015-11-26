#include <PhysicsModuleImplementation.hpp>

#include <Bullet/btBulletDynamicsCommon.h>
namespace DoremiEngine
{
    namespace Physics
    {
        PhysicsModuleImplementation::PhysicsModuleImplementation() {}
        PhysicsModuleImplementation::~PhysicsModuleImplementation() {}

        void PhysicsModuleImplementation::Startup() 
        {
        }

        void PhysicsModuleImplementation::SetWorkingDirectory(const std::string& p_workingDirectory) {}
        
        void PhysicsModuleImplementation::Shutdown() {}
    }
}

DoremiEngine::Physics::PhysicsModule* CreatePhysicsModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Physics::PhysicsModule* physics = new DoremiEngine::Physics::PhysicsModuleImplementation();
    return physics;
}