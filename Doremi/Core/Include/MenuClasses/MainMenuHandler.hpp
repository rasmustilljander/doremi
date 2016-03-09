#pragma once
// Project specific
#include <DirectXMath.h>
#include <vector>
#include <Doremi/Core/Include/MenuClasses/Button.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <Doremi/Core/Include/Helper/DoremiStates.hpp>
#include <Doremi/Core/Include/EventHandler/Subscriber.hpp>
#include <Doremi/Core/Include/MenuClasses/ScreenObject.hpp>
namespace Doremi
{
    namespace Core
    {

        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary*/
        class MainMenuHandler
        {
        public:
            MainMenuHandler(const DoremiEngine::Core::SharedContext& p_sharedContext, DirectX::XMFLOAT2 p_resolution);
            virtual ~MainMenuHandler();
            static MainMenuHandler* GetInstance();
            static void StartMainMenuHandler(const DoremiEngine::Core::SharedContext& p_sharedContext, DirectX::XMFLOAT2 p_resolution);
            int Update(double p_dt);
            // Use this order: 0 = Play, 1 = Options, 2 = Exit, (3 = Reserved, 4 = Reserved)
            void Initialize();
            auto& GetButtons() { return m_buttonList; }
            auto& GetScreenObjects() { return m_screenObjects; }
            int GetCurrentButton();

        private:
            static MainMenuHandler* m_singleton;
            const DoremiEngine::Core::SharedContext& m_sharedContext;
            std::vector<Button> m_buttonList;
            std::vector<ScreenObject*> m_screenObjects;
            ScreenObject m_background;
            ScreenObject m_menuBar;
            DirectX::XMFLOAT2 m_resolution;
            int m_currentButton;
            bool m_isFullscreen;
        };
    }
}
