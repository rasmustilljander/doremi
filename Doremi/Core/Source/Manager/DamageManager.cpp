#include <Manager/DamageManager.hpp>
namespace Doremi
{
    namespace Core
    {
        DamageManager::DamageManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "DamageManager") {}
        DamageManager::~DamageManager() {}
        void DamageManager::Update(double p_dt)
        {
            // TODOKO something
        }
    }
}