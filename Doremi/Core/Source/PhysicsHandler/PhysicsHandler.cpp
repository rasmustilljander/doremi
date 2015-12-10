#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <PhysicsHandler/PhysicsHandler.hpp>

namespace Doremi
{
    namespace Core
    {
        PhysicsHandler* PhysicsHandler::m_singleton = nullptr;

        PhysicsHandler::PhysicsHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext) {}
        PhysicsHandler::~PhysicsHandler() {}

        PhysicsHandler* PhysicsHandler::GetInstance() { return m_singleton; }

        void PhysicsHandler::StartPhysicsHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            m_singleton = new PhysicsHandler(p_sharedContext);
        }
    }
}