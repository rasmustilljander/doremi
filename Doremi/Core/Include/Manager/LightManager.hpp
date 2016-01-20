#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
namespace Doremi
{
    namespace Core
    {
        class LightManager : public Manager, public Subscriber
        {
        public:
            LightManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~LightManager();

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