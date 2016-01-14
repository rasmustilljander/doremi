#pragma once

#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
namespace Doremi
{
    namespace Core
    {
        class CharacterControlSyncManager : public Manager, public Subscriber
        {
        public:
            CharacterControlSyncManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~CharacterControlSyncManager();

            /**
            TODOJB docs*/
            void Update(double p_dt) override;

            /**
            TODOJB docs*/
            void OnEvent(Event* p_event) override;
        };
    }
}