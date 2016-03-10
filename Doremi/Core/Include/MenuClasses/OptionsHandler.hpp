#pragma once

// Project specific
#include <DirectXMath.h>
#include <vector>
#include <Doremi/Core/Include/MenuClasses/Button.hpp>
#include <Doremi/Core/Include/MenuClasses/ScreenObject.hpp>
#include <Doremi/Core/Include/MenuClasses/Bar.hpp>
#include <Doremi/Core/Include/MenuClasses/Slider.hpp>
#include <Doremi/Core/Include/MenuClasses/Text.hpp>
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

        struct DropDownResolution
        {
            Button button;
            Text text;
            uint32_t width;
            uint32_t height;
        };

        struct DropDownRefreshRate
        {
            Button button;
            Text text;
            uint32_t refreshRate;
        };

        struct DropDownMonitor
        {
            Button button;
            Text text;
            uint32_t monitorIndex;
        };

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
            auto& GetText() { return m_text; }
            auto& GetButtons() { return m_optionsButtons; }
            auto& GetDropDownResolutions() { return m_dropdownResolution; }
            auto& GetDropDownRefresh() { return m_dropdownRefresh; }
            auto& GetDropDownMonitors() { return m_dropdownMonitors; }
            auto& GetSliders() { return m_sliders; }

        private:
            explicit OptionsHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            ~OptionsHandler();

            void CreateButtons(const DoremiEngine::Core::SharedContext& p_sharedContext);

            void CreateScreenResolutionOption(const DoremiEngine::Core::SharedContext& p_sharedContext);

            void CreateRefreshOption(const DoremiEngine::Core::SharedContext& p_sharedContext);

            void CreateMonitorOption(const DoremiEngine::Core::SharedContext& p_sharedContext);

            void CreateSliders(const DoremiEngine::Core::SharedContext& p_sharedContext);

            void CreateResolutionDropDown();

            void CreateRefreshRateDropDown();

            void ClearResolutionDropDown();

            void UpdateResolution();

            void UpdateRefreshRate();

            void ClearRefreshDropDown();

            void CreateMonitorDropDown();

            void ClearMonitorDropDown();

            void UpdateSliderEffect(const SliderEffect& p_sliderEffect, const float& percent);

            static OptionsHandler* m_singleton;
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            Bar m_soundBar;

            Button m_resolutionButton;
            Text m_resolutionText;

            Button m_refreshButton;
            Text m_refreshText;

            Button m_monitorButton;
            Text m_monitorText;

            Button m_applyButton;
            Button m_cancelButton;

            Button* m_highlightedButton;

            // only for drop downs to get info
            uint32_t m_highlightedWidth;
            uint32_t m_highlightedHegiht;

            uint32_t m_highlightedRefreshrate;

            uint32_t m_highlightedMonitorindex;

            std::vector<ScreenObject*> m_screenObjects;
            std::vector<Bar*> m_bars;
            std::vector<Button*> m_optionsButtons;
            std::vector<Text*> m_text;

            std::vector<Slider*> m_sliders;

            std::vector<DropDownResolution> m_dropdownResolution;
            std::vector<DropDownRefreshRate> m_dropdownRefresh;
            std::vector<DropDownMonitor> m_dropdownMonitors;


            bool t_resolutionDropdownIsActive;
            bool t_refreshDropdownIsActive;
            bool t_monitorDropdownIsActive;

            // std::map<uint32_t, std::map<DisplayMode, std::vector<uint32_t>>> m_displayModeMap;
            uint32_t m_currentResolutionWidth;
            uint32_t m_currentResolutionHeight;

            uint32_t m_currentMonitor;
            uint32_t m_currentRefreshRate;
        };
    }
}
