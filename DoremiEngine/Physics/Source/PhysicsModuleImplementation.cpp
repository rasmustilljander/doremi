#include <PhysicsModuleImplementation.hpp>

#include <iostream>
namespace DoremiEngine
{
    namespace Physics
    {
        PhysicsModuleImplementation::PhysicsModuleImplementation() {}
        PhysicsModuleImplementation::~PhysicsModuleImplementation() {}

        /**
        TODO
        */
        void PhysicsModuleImplementation::Startup() {}

        /**
        TODO
        */
        void PhysicsModuleImplementation::SetWorkingDirectory(const std::string& p_workingDirectory) {}

        /**
        TODO
        */
        void PhysicsModuleImplementation::Shutdown() {}

        // Only an example method to demonstrate how the engine is used
        float PhysicsModuleImplementation::ExampleMethod(const float& posx) 
        {
            return posx * 400;
        }
    }
}

DoremiEngine::Physics::PhysicsModule* CreatePhysicsModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Physics::PhysicsModule* physics = new DoremiEngine::Physics::PhysicsModuleImplementation();
    return physics;
}