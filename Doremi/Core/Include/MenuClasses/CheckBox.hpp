#pragma once
// Project specific
#include <DirectXMath.h>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Texture/SpriteInfo.hpp>
#include <Doremi/Core/Include/Helper/DoremiStates.hpp>
#include <Doremi/Core/Include/MenuClasses/Button.hpp>

namespace Doremi
{
    namespace Core
    {
        struct CheckBoxMaterials
        {
            DoremiEngine::Graphic::MaterialInfo* m_vanillaMaterial;
            DoremiEngine::Graphic::MaterialInfo* m_highLightedMaterial;
            DoremiEngine::Graphic::MaterialInfo* m_selectedLightedMaterial;
        };

        enum class CheckBoxActions
        {
            FULLSCREEN,
        };

        using namespace DirectX;

        /**
        Only an example of what a manager might look like
        Doesn't do anything, and could be removed once examples are no longer necessary
        */
        class CheckBox
        {
        public:
            CheckBox(Button p_button, Core::CheckBoxActions p_menuState);

            CheckBox();

            ~CheckBox();

            // Checks if mouse is inside this CheckBox
            bool CheckIfInside(float p_mousePosX, float p_mousePosY);

            void TryHightlight();

            void SetPressedByBool(bool p_isPressed);

            void TogglePress();

            Core::CheckBoxActions m_CheckBoxAction;

            Button m_button;

            bool m_isPressed;

        private:
        };
    }
}
