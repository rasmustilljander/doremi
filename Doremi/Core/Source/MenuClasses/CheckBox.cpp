// Project specific
#include <Doremi/Core/Include/MenuClasses/CheckBox.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <Doremi/Core/Include/Helper/DoremiStates.hpp>

// Third party

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;
        CheckBox::CheckBox(Button p_button, Core::CheckBoxActions p_menuState) : m_button(p_button), m_CheckBoxAction(p_menuState), m_isPressed(false)
        {
        }


        CheckBox::CheckBox() {}

        CheckBox::~CheckBox() {}

        bool CheckBox::CheckIfInside(float p_mousePosX, float p_mousePosY)
        {

            DoremiEngine::Graphic::SpriteData& t_data = m_button.m_spriteInfo->GetData();

            // Check if to the right of the CheckBox
            if((p_mousePosX > t_data.position.x + t_data.origo.x + t_data.halfsize.x) || (p_mousePosX < t_data.position.x + t_data.origo.x - t_data.halfsize.x) ||
               (p_mousePosY < t_data.position.y + t_data.origo.y - t_data.halfsize.y) || (p_mousePosY > t_data.position.y + t_data.origo.y + t_data.halfsize.y))
            {
                if(!m_isPressed)
                {
                    m_button.m_materialInfo = m_button.m_buttonMaterials.m_vanillaMaterial;
                }

                return false;
            }
            // Otherwise we are on top of this CheckBox
            else
            {
                if(!m_isPressed)
                {
                    m_button.m_materialInfo = m_button.m_buttonMaterials.m_highLightedMaterial;
                }

                return true;
            }
        }

        void CheckBox::TryHightlight()
        {
            if(!m_isPressed)
            {
                m_button.m_materialInfo = m_button.m_buttonMaterials.m_highLightedMaterial;
            }
        }

        void CheckBox::SetPressedByBool(bool p_isPressed)
        {
            m_isPressed = p_isPressed;
            if(m_isPressed)
            {
                m_button.m_materialInfo = m_button.m_buttonMaterials.m_selectedLightedMaterial;
            }
            else
            {
                m_button.m_materialInfo = m_button.m_buttonMaterials.m_vanillaMaterial;
            }
        }

        void CheckBox::TogglePress()
        {
            m_isPressed = !m_isPressed;
            if(m_isPressed)
            {
                m_button.m_materialInfo = m_button.m_buttonMaterials.m_selectedLightedMaterial;
            }
            else
            {
                m_button.m_materialInfo = m_button.m_buttonMaterials.m_highLightedMaterial;
            }
        }
    }
}