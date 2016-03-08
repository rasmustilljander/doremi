#pragma once
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
namespace Doremi
{
    namespace Core
    {
        class LifeTimeManager : public Manager
        {
        public:
            explicit LifeTimeManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~LifeTimeManager();
            void Update(double p_dt) override;
        };
    }
}