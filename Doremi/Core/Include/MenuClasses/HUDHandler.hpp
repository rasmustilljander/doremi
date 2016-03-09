#pragma once

// Project specific
#include <DirectXMath.h>
#include <vector>
#include <Doremi/Core/Include/MenuClasses/Button.hpp>
#include <Doremi/Core/Include/MenuClasses/ScreenObject.hpp>
#include <Doremi/Core/Include/MenuClasses/Bar.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        struct ExpirableScreenObject
        {
            ScreenObject Object;
            float LifeTime;
            float MaxLifeTime;
        };
        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary
        */
        class HUDHandler : public Subscriber
        {
        public:
            static HUDHandler* GetInstance();
            static void StartHUDHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            void Update(double p_dt);
            void OnEvent(Event* p_event) override;

            auto& GetScreenObjects() { return m_screenObjects; }
            auto& GetBars() { return m_bars; }

        private:
            explicit HUDHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~HUDHandler();

            static HUDHandler* m_singleton;
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            ExpirableScreenObject m_bloodScreen;

            Bar m_healthBar;

            std::vector<ScreenObject*> m_screenObjects;
            std::vector<Bar*> m_bars;
        };
    }
}
