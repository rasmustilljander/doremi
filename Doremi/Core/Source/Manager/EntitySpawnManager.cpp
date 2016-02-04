/// Project specific
// This class
#include <Manager/EntitySpawnManager.hpp>
// Handlers
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/EntityHandlerServer.hpp>
#include <EventHandler/EventHandler.hpp>
#include <PlayerHandler.hpp>
// Components
#include <EntityComponent/Components/EntitySpawnerComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/EntityTypeComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>
#include <EntityComponent/Components/PotentialFieldComponent.hpp>
// Events
#include <EventHandler/Events/EntityCreatedEvent.hpp>

/// Engine
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>
#include <DoremiEngine/Physics/Include/CharacterControlManager.hpp>
// AI
#include <DoremiEngine/AI/Include/Interface/SubModule/PotentialFieldSubModule.hpp>
#include <DoremiEngine/AI/Include/Interface/PotentialField/PotentialFieldActor.hpp>
#include <DoremiEngine/AI/Include/AIModule.hpp>
/// Third party
#include <DirectXMath.h>
using namespace DirectX;

// DEBUG
#include <iostream>
using namespace std;

namespace Doremi
{
    namespace Core
    {
        EntitySpawnManager::EntitySpawnManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "EntitySpawnManager")
        {
        }

        EntitySpawnManager::~EntitySpawnManager() {}


        void EntitySpawnManager::Update(double p_dt)
        {
			static int DEBUGcount = 0;
            EntityHandler& entityHandler = EntityHandler::GetInstance();
            // Loop through all entities
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            for(size_t i = 0; i < length; i++)
            {
                // Check that the current entity has the relevant components
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::EntitySpawner) | (int)ComponentType::Transform)
                {
                    // We've found an entity spawner
                    EntitySpawnComponent* spawnComp = EntityHandler::GetInstance().GetComponentFromStorage<EntitySpawnComponent>(i);
                    // Check if its a timed spawned
                    if(spawnComp->type == SpawnerType::TimedSpawner)
                    {
                        // Update time since last spawn
                        spawnComp->timeSinceLastSpawn += p_dt;
                        // Check if it's time to spawn and if we haven't already spawned the max number
                        if(spawnComp->timeSinceLastSpawn >= spawnComp->timeBetweenSpawns && spawnComp->currentNumSpawnedEntities < spawnComp->maxNumSpawnedEntites)
                        {
                            // Hax to ensure nothing spawns when no players are active, which apparently is a no-can-do
                            if(PlayerHandler::GetInstance()->GetPlayerMap().size() > 0)
                            {
                                // We should spawn something
                                CreateEntity(spawnComp->entityBlueprint, i);
                                spawnComp->currentNumSpawnedEntities++;
                            }
                            // Reset timer
                            spawnComp->timeSinceLastSpawn = 0;
                        }
                        else
                        {
                            // Nothing
                        }
                    }
                    else
                    {
                        // nothing
                    }
                }
            }
        }

        void EntitySpawnManager::OnEvent(Event* p_event) {}

        void EntitySpawnManager::CreateEntity(Blueprints p_blueprint, int p_spawnerID)
        {
            EntityHandler& entityHandler = EntityHandler::GetInstance();
            switch(p_blueprint)
            {
                case Blueprints::EnemyEntity:
                    TransformComponent* transComp = entityHandler.GetComponentFromStorage<TransformComponent>(p_spawnerID);
                    // Spawn inside the spawner. This might be changed in the future
                    XMFLOAT3 spawnPosition = transComp->position;
                    int newID = EntityHandlerServer::GetInstance().CreateEntity(Blueprints::EnemyEntity, spawnPosition);
                    int matID = Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PhysicsMaterialComponent>(newID)->p_materialID;
                    m_sharedContext.GetPhysicsModule().GetCharacterControlManager().AddController(newID, matID, spawnPosition, XMFLOAT2(0.1f, 0.5f));
                    m_sharedContext.GetPhysicsModule().GetCharacterControlManager().SetDrain(newID, true);
                    Core::PotentialFieldComponent* potentialComponent =
                        Core::EntityHandler::GetInstance().GetComponentFromStorage<Core::PotentialFieldComponent>(newID);
                    potentialComponent->ChargedActor =
                        m_sharedContext.GetAIModule().GetPotentialFieldSubModule().CreateNewActor(spawnPosition, -1.0f, 3.0f, false,
                                                                                                  DoremiEngine::AI::AIActorType::Enemy);

                    EntityCreatedEvent* AIGroupActorCreated = new Core::EntityCreatedEvent(newID, Core::EventType::AiGroupActorCreation);
                    EventHandler::GetInstance()->BroadcastEvent(AIGroupActorCreated);
                    break;
            }
        }
    }
}