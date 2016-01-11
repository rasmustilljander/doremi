// Project specific
#include <Doremi/Core/Include/MenuClasses/Button.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <Doremi/Core/Include/Helper/MenuStates.hpp>

// Third party

// Standard
#include <iostream>

using namespace std;

namespace Doremi
{
    namespace Core
    {
        using namespace DirectX;
        Button::Button(const XMFLOAT2& p_position, const XMFLOAT2& p_size, DoremiEngine::Graphic::MaterialInfo* p_materialInfo,
                       DoremiEngine::Graphic::MeshInfo* p_meshInfo, MenuStates::MenuState p_menuState)
            : m_position(p_position), m_size(p_size), m_materialInfo(p_materialInfo), m_meshInfo(p_meshInfo), m_menuState(p_menuState)
        {
        }
        Button::Button() {}
        Button::~Button() {}
        bool Button::CheckIfInside(int p_mousePosX, int p_mousePosY)
        {
            m_mousePos.x = p_mousePosX;
            m_mousePos.y = p_mousePosY;
            // Check if to the right of the button
            if(p_mousePosX > m_position.x + m_size.x)
            {
                return false;
            }
            // Check if mouse is to the left of the button
            else if(p_mousePosX < m_position.x)
            {
                return false;
            }
            // Check if mouse is over the button
            else if(p_mousePosY < m_position.y)
            {
                return false;
            }
            // Check if mouse is under the button
            else if(p_mousePosY > m_position.y + m_size.y)
            {
                return false;
            }
            // Otherwise we are on top of this button
            else
            {
                return true;
            }
        }
    }
}