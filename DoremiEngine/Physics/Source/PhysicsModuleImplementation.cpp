#include <PhysicsModuleImplementation.hpp>

#include <PhysxSubmodule.Hpp>

namespace DoremiEngine
{
    namespace Physics
    {
        PhysicsModuleImplementation::PhysicsModuleImplementation(const Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}
        PhysicsModuleImplementation::~PhysicsModuleImplementation() {}

        void PhysicsModuleImplementation::Startup()
        {
            // Create submodule
            m_physX = new PhysXSubmodule();
            // Actually init PhysX
            m_physX->InitPhysx();
        }

        void PhysicsModuleImplementation::Shutdown() {}

        void PhysicsModuleImplementation::Update(float p_dt) { m_physX->Update(p_dt); }

        // Only an example method to demonstrate how the engine is used
        float PhysicsModuleImplementation::ExampleMethod(const float& posx) { return 1; }

        void PhysicsModuleImplementation::ExampleMethodAltersData(float* p_posX, float* p_posY)
        {
            *p_posX = 5;
            *p_posY = *p_posX * 2;
        }

        void PhysicsModuleImplementation::StartPhysX()
        {
            // Create a start scene. TODOJB move somewhere else?
            m_physX->CreateTestScene();
            float boxDims = 0.5;
            m_physX->AddBox(0, 1, 0, boxDims, boxDims, boxDims);
        }
    }
}

DoremiEngine::Physics::PhysicsModule* CreatePhysicsModule(const DoremiEngine::Core::SharedContext& p_sharedContext)
{
    DoremiEngine::Physics::PhysicsModule* physics = new DoremiEngine::Physics::PhysicsModuleImplementation(p_sharedContext);
    return physics;
}