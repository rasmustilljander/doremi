#include <Doremi/Core/Include/PlayerSpawnerHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/TriggerEvent.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/TransformComponent.hpp>
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/PlayerRespawnEvent.hpp>
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/HealthComponent.hpp>
#include <iostream>

namespace Doremi
{
    namespace Core
    {
        PlayerSpawnerHandler* PlayerSpawnerHandler::m_singleton = nullptr;

        PlayerSpawnerHandler* PlayerSpawnerHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::cout << "Called GetInstance befor StartupPlayerSpawnerHandler was called" << std::endl;
            }
            return m_singleton;
        }

        void PlayerSpawnerHandler::StartupPlayerSpawnerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("StartupPlayerSpawnerHandler was called");
            }
            m_singleton = new PlayerSpawnerHandler(p_sharedContext);
        }

        PlayerSpawnerHandler::PlayerSpawnerHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            EventHandler::GetInstance()->Subscribe(EventType::Trigger, this);
        }

        PlayerSpawnerHandler::~PlayerSpawnerHandler() {}

        EntityID PlayerSpawnerHandler::GetCurrentSpawnerEntityID() { return m_currentPlayerSpawner; }

        void PlayerSpawnerHandler::SetCurrentSpawner(EntityID p_entityID) { m_currentPlayerSpawner = p_entityID; }

        void PlayerSpawnerHandler::RespawnPlayer(EntityID p_entityID)
        {
            DoremiEngine::Physics::CharacterControlManager& t_characterBodyManager = m_sharedContext.GetPhysicsModule().GetCharacterControlManager();
            DoremiEngine::Physics::RigidBodyManager& t_rigidBodyManager = m_sharedContext.GetPhysicsModule().GetRigidBodyManager();

            // Get position and orientation of the trigger..
            DirectX::XMFLOAT3 t_triggerPosition = t_rigidBodyManager.GetBodyPosition(m_currentPlayerSpawner);
            DirectX::XMFLOAT4 t_triggerOrientation = t_rigidBodyManager.GetBodyOrientation(m_currentPlayerSpawner);

            // ..and set it to the players position, orientation
            t_characterBodyManager.SetPosition(p_entityID, t_triggerPosition);

            // Since we don't know if this will be called befor sync or not, we set it to the transform as well
            TransformComponent* t_transComp = GetComponent<TransformComponent>(p_entityID);
            t_transComp->position = t_triggerPosition;
            t_transComp->rotation = t_triggerOrientation;

            // Reset health
            HealthComponent* t_healthComp = GetComponent<HealthComponent>(p_entityID);
            t_healthComp->currentHealth = t_healthComp->maxHealth;


            // Create event for other purposes
            PlayerRespawnEvent* t_playerRespawn = new PlayerRespawnEvent(p_entityID, t_triggerPosition, t_triggerOrientation);
            EventHandler::GetInstance()->BroadcastEvent(t_playerRespawn);
        }

        void PlayerSpawnerHandler::OnEvent(Event* p_event)
        {
            // if(p_event->eventType == EventType::Trigger)
            //{
            //    TriggerEvent* t_triggEvent = static_cast<TriggerEvent*>(p_event);

            //    // If spawn point trigger
            //    if(t_triggEvent->triggerType == TriggerType::NewSpawnPointTrigger)
            //    {
            //        // Could get who got the spawner here as well? If we want player specific spawners
            //        m_currentPlayerSpawner = t_triggEvent->triggerEntityID;
            //    }
            //}
        }
    }
}