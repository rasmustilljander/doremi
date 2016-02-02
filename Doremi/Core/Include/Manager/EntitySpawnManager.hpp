#pragma once
// Project specific
#include <Manager/Manager.hpp>
#include <EventHandler/Subscriber.hpp>

namespace Doremi
{
    namespace Core
    {
        /**
        Manager to create entities. So far it does it either after a period
        of time has elapsed, or after a certain event has been sent. Should
        most likely be expanded in the future.*/
        class EntitySpawnManager : public Manager, public Subscriber
        {
        public:
            EntitySpawnManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~EntitySpawnManager();
            void Update(double p_dt) override;
            void OnEvent(Event* p_event) override;
        };
    }
}
