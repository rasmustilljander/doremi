/// Project specific
// This class
#include <Manager/EntitySpawnManager.hpp>
// Handlers
#include <EntityComponent/EntityHandler.hpp>
// Components
#include <EntityComponent/Components/EntitySpawnerComponent.hpp>

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
            // Loop through all entities
            const size_t length = EntityHandler::GetInstance().GetLastEntityIndex();
            for(size_t i = 0; i < length; i++)
            {
                // Check that the current entity has the relevant components
                if(EntityHandler::GetInstance().HasComponents(i, (int)ComponentType::EntitySpawner))
                {
                    // We've found an entity spawner
                    EntitySpawnComponent* spawnComp = EntityHandler::GetInstance().GetComponentFromStorage<EntitySpawnComponent>(i);
                    // Check if its a timed spawned
                    if(spawnComp->type == SpawnerType::TimedSpawner)
                    {
                        // Update time since last spawn
                        spawnComp->timeSinceLastSpawn += p_dt;
                        // Check if it's time to spawn
                        if(spawnComp->timeSinceLastSpawn >= spawnComp->timeBetweenSpawns)
                        {
                            // We should spawn something
                        }
                    }
                }
            }
        }
        void EntitySpawnManager::OnEvent(Event* p_event) {}
    }
}