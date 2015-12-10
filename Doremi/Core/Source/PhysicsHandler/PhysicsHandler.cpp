#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <PhysicsHandler/PhysicsHandler.hpp>

#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>

namespace Doremi
{
    namespace Core
    {
        PhysicsHandler* PhysicsHandler::m_singleton = nullptr;

        PhysicsHandler::PhysicsHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) 
            : m_sharedContext(p_sharedContext) 
        {
            m_sharedContext.GetPhysicsModule().StartPhysX(); //TODOJB probably silly to have here
        }
        PhysicsHandler::~PhysicsHandler() {}

        PhysicsHandler* PhysicsHandler::GetInstance() { return m_singleton; }

        void PhysicsHandler::StartPhysicsHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            m_singleton = new PhysicsHandler(p_sharedContext);
        }

        void PhysicsHandler::BigTestFunction()
        {
            // Non-accurate function name. This creates a physics world (i hope)
            m_sharedContext.GetPhysicsModule().StartPhysX();
        }

        void PhysicsHandler::UpdatePhysics(float p_dt)
        {
            m_sharedContext.GetPhysicsModule().Update(p_dt);
        }
    }
}