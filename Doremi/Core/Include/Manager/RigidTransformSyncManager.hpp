#pragma once
// Project specific
#include <Manager/Manager.hpp>
#include <EventHandler/Subscriber.hpp>
namespace Doremi
{
    namespace Core
    {
        class RigidTransformSyncManager : public Manager, public Subscriber
        {
            public:
            RigidTransformSyncManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~RigidTransformSyncManager();

            /**
            Walks thorugh every entity and tells the engine to render every entity with a render component
            */
            void Update(double p_dt) override;

            /**
            TODOKO check is we need any event and write doc, could be needed for particles
            */
            void OnEvent(Event* p_event) override;
        };
    }
}
