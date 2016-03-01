// Project specific
#include <Doremi/Core/Include/MenuClasses/Button.hpp>
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
        Button::Button(ButtonMaterials p_buttonMaterials, DoremiEngine::Graphic::SpriteInfo* p_spriteInfo, Core::DoremiButtonActions p_menuState)
            : m_buttonMaterials(p_buttonMaterials), m_spriteInfo(p_spriteInfo), m_buttonAction(p_menuState)
        {
            // Set current
            m_materialInfo = m_buttonMaterials.m_vanillaMaterial;
        }

        Button::Button() {}

        Button::~Button() {}

        bool Button::CheckIfInside(float p_mousePosX, float p_mousePosY)
        {
            DoremiEngine::Graphic::SpriteData& t_data = m_spriteInfo->GetData();

            // Check if to the right of the button
            if(p_mousePosX > t_data.position.x + t_data.origo.x + t_data.halfsize.x)
            {
                m_materialInfo = m_buttonMaterials.m_vanillaMaterial;
                return false;
            }
            // Check if mouse is to the left of the button
            else if(p_mousePosX < t_data.position.x + t_data.origo.x - t_data.halfsize.x)
            {
                m_materialInfo = m_buttonMaterials.m_vanillaMaterial;
                return false;
            }
            // Check if mouse is over the button
            else if(p_mousePosY < t_data.position.y + t_data.origo.y - t_data.halfsize.y)
            {
                m_materialInfo = m_buttonMaterials.m_vanillaMaterial;
                return false;
            }
            // Check if mouse is under the button
            else if(p_mousePosY > t_data.position.y + t_data.origo.y + t_data.halfsize.y)
            {
                m_materialInfo = m_buttonMaterials.m_vanillaMaterial;
                return false;
            }
            // Otherwise we are on top of this button
            else
            {
                m_materialInfo = m_buttonMaterials.m_highLightedMaterial;
                return true;
            }
            return false;
        }
    }
}