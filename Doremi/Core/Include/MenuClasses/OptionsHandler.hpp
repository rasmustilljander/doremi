#pragma once

// Project specific
#include <DirectXMath.h>
#include <vector>
#include <Doremi/Core/Include/MenuClasses/Button.hpp>
#include <Doremi/Core/Include/MenuClasses/ScreenObject.hpp>
#include <Doremi/Core/Include/MenuClasses/Bar.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>

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

        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary*/
        class OptionsHandler
        {
        public:
            static OptionsHandler* GetInstance();
            static void StartOptionsHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            void Update(double p_dt);

            auto& GetScreenObjects() { return m_screenObjects; }
            auto& GetBars() { return m_bars; }

        private:
            explicit OptionsHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~OptionsHandler();

            void CreateScreenResolutionOption(const DoremiEngine::Core::SharedContext& p_sharedContext);

            static OptionsHandler* m_singleton;
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            Bar m_soundBar;

            Button m_leftResolutionButton;
            Button m_rightResolutionButton;
            Button m_middleResolutionButton;

            Button* m_highlightedButton;

            std::vector<ScreenObject> m_screenObjects;
            std::vector<Bar*> m_bars;
            std::vector<Button*> m_menuButtons;

            std::vector<Button*> m_toDrawButtons;

            bool t_resolutionDropdownIsActive;

            // std::map<uint32_t, std::map<DisplayMode, std::vector<uint32_t>>> m_displayModeMap;
        };
    }
}
