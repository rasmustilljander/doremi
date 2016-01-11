#pragma once
// Project specific
#include <Doremi/Core/Include/Manager/Manager.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>

namespace Doremi
{
    namespace Core
    {
        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary*/
        class AudioManager : public Manager, public Subscriber
        {
            public:
            AudioManager(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~AudioManager();
            void Update(double p_dt) override;
            // If the manager is a Subscriber, inherits the subscriber class, it will have to override OnEvent funciton
            void OnEvent(Event* p_event) override;

            // The current dominant frequency from the analyser
            float m_dominantFrequency;
            // Is the reloadbutton pressed?
            bool m_gunReloadButtonDown;
            // The ammount of time that the gunbutton has been pressed down
            double m_timeThatGunButtonIsDown;

            private:
        };
    }
}
