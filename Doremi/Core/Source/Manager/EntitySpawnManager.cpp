/// Project specific
// This class
#include <Manager/EntitySpawnManager.hpp>

namespace Doremi
{
    namespace Core
    {
        EntitySpawnManager::EntitySpawnManager(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : Manager(p_sharedContext, "EntitySpawnManager")
        {
        }

        EntitySpawnManager::~EntitySpawnManager() {}


        void EntitySpawnManager::Update(double p_dt) {}
        void EntitySpawnManager::OnEvent(Event* p_event) {}
    }
}