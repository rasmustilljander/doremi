#include <Doremi/Core/Include/HealthChecker.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/HealthComponent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/RemoveEntityEvent.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/PlayerHandlerServer.hpp>
#include <Doremi/Core/Include/PlayerSpawnerHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/DamageTakenEvent.hpp>
#include <Doremi/Core/Include/EventHandler/Events/AnimationTransitionEvent.hpp>

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

        HealthChecker::HealthChecker() { EventHandler::GetInstance()->Subscribe(EventType::DamageTaken, this); }

        HealthChecker::~HealthChecker() {}

        void HealthChecker::Update()
        {
            EntityHandler& t_entityHandler = EntityHandler::GetInstance();
            PlayerHandlerServer* t_playerHandler = static_cast<PlayerHandlerServer*>(PlayerHandler::GetInstance());
            PlayerSpawnerHandler* t_playerRespawnHandler = PlayerSpawnerHandler::GetInstance();

            size_t NumEntities = t_entityHandler.GetLastEntityIndex();

            // Loop over all entities, find health and check if remove
            for(size_t entityID = 0; entityID < NumEntities; entityID++)
            {
                if(t_entityHandler.HasComponents(entityID, (uint32_t)ComponentType::Health))
                {
                    if(GetComponent<HealthComponent>(entityID)->currentHealth <= 0)
                    {
                        // If we are not player, we should be removed
                        if(!t_playerHandler->IsPlayer(entityID))
                        {
                            EntityHandler::GetInstance().RemoveEntity(entityID);
                        }
                        else // if we're player we should respawn
                        {
                            t_playerRespawnHandler->RespawnPlayer(entityID);
                        }
                    }
                }
            }
        }
        void HealthChecker::OnEvent(Event* p_event)
        {
            if(p_event->eventType == EventType::DamageTaken)
            {
                DamageTakenEvent* t_dmgTakenEvent = static_cast<DamageTakenEvent*>(p_event);

                HealthComponent* t_healthComp = GetComponent<HealthComponent>(t_dmgTakenEvent->entityId);
                t_healthComp->currentHealth -= t_dmgTakenEvent->damage;
            }
        }
    }
}