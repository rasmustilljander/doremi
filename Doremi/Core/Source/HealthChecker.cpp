#include <Doremi/Core/Include/HealthChecker.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/HealthComponent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/RemoveEntityEvent.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>

namespace Doremi
{
    namespace Core
    {
        HealthChecker* HealthChecker::m_singleton = nullptr;

        HealthChecker* HealthChecker::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                m_singleton = new HealthChecker();
            }
            return m_singleton;
        }

        HealthChecker::HealthChecker() {}

        HealthChecker::~HealthChecker() {}

        void HealthChecker::Update()
        {
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();
            size_t NumEntities = t_entityHandler.GetLastEntityIndex();

            // Loop over all entities, find health and check if remove
            for(size_t entityID = 0; entityID < NumEntities; entityID++)
            {
                if(t_entityHandler.HasComponents(entityID, (uint32_t)ComponentType::Health))
                {
                    if(GetComponent<HealthComponent>(entityID)->currentHealth <= 0)
                    {
                        // EventHandler::GetInstance()->BroadcastEvent(new RemoveEntityEvent(entityID));
                    }
                }
            }
        }
    }
}