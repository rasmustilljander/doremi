#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>

namespace Doremi
{
    namespace Core
    {
        /**
        Manager to handle Pressured particle systems.
        Basically, all it does is read the positions and then render the same texture as
        the emitter has, on top of the positions. This will most likely have to be changed
        later.*/
        class PressureParticleManager : public Manager, public Subscriber
        {
            public:
            PressureParticleManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~PressureParticleManager();
            void Update(double p_dt) override;
            // If the manager is a Subscriber, inherits the subscriber class, it will have to override OnEvent funciton
            void OnEvent(Event* p_event) override;

            private:
        };
    }
}
