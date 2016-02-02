/// Project specific
// This class
#include <Manager/EntitySpawnManager.hpp>
// Handlers
#include <EntityComponent/EntityHandler.hpp>
// Components
#include <EntityComponent/Components/EntitySpawner.hpp>

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
                }
            }
        }
        void EntitySpawnManager::OnEvent(Event* p_event) {}
    }
}