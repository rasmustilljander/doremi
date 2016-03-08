#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
namespace Doremi
{
    namespace Core
    {
        /**
        This manager updates the extra drains on some entities*/
        class ExtraDrainSyncManager : public Manager, public Subscriber
        {
        public:
            explicit ExtraDrainSyncManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~ExtraDrainSyncManager();
            void Update(double p_dt) override;
            void OnEvent(Event* p_event) override;
        };
    }
}
