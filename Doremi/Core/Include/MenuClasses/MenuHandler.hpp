#pragma once
// Project specific
#include <DirectXMath.h>
#include <vector>
#include <Doremi/Core/Include/MenuClasses/Button.hpp>
#include <Doremi/Core/Include/InputHandlerClient.hpp>
#include <Doremi/Core/Include/Helper/MenuStates.hpp>
namespace Doremi
{
    namespace Core
    {
        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary*/
        class MenuHandler
        {
            public:
            MenuHandler(const DoremiEngine::Core::SharedContext& p_sharedContext, DirectX::XMFLOAT2 p_resolution);
            virtual ~MenuHandler();
            static MenuHandler* GetInstance();
            static void StartMenuHandler(const DoremiEngine::Core::SharedContext& p_sharedContext, DirectX::XMFLOAT2 p_resolution);
            int Update(double p_dt);
            // Use this order: 0 = Play, 1 = Options, 2 = Exit, (3 = Reserved, 4 = Reserved)
            void Initialize(std::vector<std::string> p_buttonTextureNames);
            std::vector<Button> GetButtons();

            private:
            static MenuHandler* m_singleton;
            const DoremiEngine::Core::SharedContext& m_sharedContext;
            std::vector<Button> m_buttonList;
            DirectX::XMFLOAT2 m_resolution;
            int m_currentButton;
            InputHandlerClient* m_inputHandler;
            MenuStates::MenuState m_menuState;
        };
    }
}
