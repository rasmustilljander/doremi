/// Project specific
// This class
#include <Manager/EntitySpawnManager.hpp>
// Handlers
#include <EntityComponent/EntityHandler.hpp>
#include <EntityComponent/EntityHandlerServer.hpp>
// Components
#include <EntityComponent/Components/EntitySpawnerComponent.hpp>
#include <EntityComponent/Components/PhysicsMaterialComponent.hpp>
#include <EntityComponent/Components/RigidBodyComponent.hpp>
#include <EntityComponent/Components/EntityTypeComponent.hpp>
#include <EntityComponent/Components/TransformComponent.hpp>

/// Engine
// Physics
#include <DoremiEngine/Physics/Include/PhysicsModule.hpp>
#include <DoremiEngine/Physics/Include/PhysicsMaterialManager.hpp>
#include <DoremiEngine/Physics/Include/RigidBodyManager.hpp>

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
                        cout << spawnComp->timeSinceLastSpawn;
                        // Check if it's time to spawn
                        if(spawnComp->timeSinceLastSpawn >= spawnComp->timeBetweenSpawns)
                        {
                            // Reset timer
                            spawnComp->timeSinceLastSpawn = 0;
                            // cout << "spawning dude" << endl;
                            // We should spawn something
                            /// DEBUG STUFF. Work in progress
                            // Make it into an enemy bullet just to see that it works
                            TransformComponent* transComp = entityHandler.GetComponentFromStorage<TransformComponent>(i);
                            XMFLOAT3 spawnPosition = transComp->position;
                            spawnPosition.y += 10;
                            int enemyID = EntityHandlerServer::GetInstance().CreateEntity(Blueprints::BulletEntity, spawnPosition);
                            PhysicsMaterialComponent* matComp = entityHandler.GetComponentFromStorage<PhysicsMaterialComponent>(i);
                            matComp->p_materialID = m_sharedContext.GetPhysicsModule().GetPhysicsMaterialManager().CreateMaterial(0, 0, 0);
                            m_sharedContext.GetPhysicsModule().GetRigidBodyManager().AddBoxBodyDynamic(i, spawnPosition, XMFLOAT4(0, 0, 0, 1),
                                                                                                       XMFLOAT3(0.25, 0.25, 0.25), matComp->p_materialID);

                            EntityTypeComponent* typeComp = entityHandler.GetComponentFromStorage<EntityTypeComponent>(i);
                            typeComp->type = EntityType::EnemyBullet;
                        }
                        else
                        {
                            // Nothing
                        }
                    }
                }
            }
        }
        void EntitySpawnManager::OnEvent(Event* p_event) {}
    }
}