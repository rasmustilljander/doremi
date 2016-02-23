#include <Manager/LifeTimeManager.hpp>
#include <EntityComponent/EntityHandler.hpp>

#include <EntityComponent/Components/LifeTimeComponent.hpp>

namespace Doremi
{
    namespace Core
    {
        LifeTimeManager::LifeTimeManager(const DoremiEngine::Core::SharedContext& p_sharedContext) : Manager(p_sharedContext, "LifeTimeManager") {}
        LifeTimeManager::~LifeTimeManager() {}
        void LifeTimeManager::Update(double p_dt)
        {
            EntityHandler& entityHandler = EntityHandler::GetInstance();
            const size_t length = entityHandler.GetLastEntityIndex();
            int mask = (int)ComponentType::LifeTime;
            for(size_t i = 0; i < length; i++)
            {
                if(entityHandler.HasComponents(i, mask))
                {
                    LifeTimeComponent* lifeTimeComp = entityHandler.GetComponentFromStorage<LifeTimeComponent>(i);
                    lifeTimeComp->LifeTime -= p_dt;
                    if(lifeTimeComp->LifeTime <= 0)
                    {
                        entityHandler.RemoveEntity(i);
                    }
                }
            }
        }
    }
}