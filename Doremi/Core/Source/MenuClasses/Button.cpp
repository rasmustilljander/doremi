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
        Button::Button(const XMFLOAT2& p_position, const XMFLOAT2& p_size, ButtonMaterials p_buttonMaterials,
                       DoremiEngine::Graphic::MeshInfo* p_meshInfo, Core::DoremiButtonActions p_menuState)
            : m_position(p_position), m_size(p_size), m_buttonMaterials(p_buttonMaterials), m_meshInfo(p_meshInfo), m_buttonAction(p_menuState)
        {
            // Building a transform matrix. needs no rotation or scaling orientation the variable can be reused. Scaling origin is 0,0,0 for the quad
            m_materialInfo = m_buttonMaterials.m_vanillaMaterial;
            XMVECTOR t_origin = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f));
            XMVECTOR t_quater = XMLoadFloat4(&XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
            XMVECTOR t_scaling = XMLoadFloat3(&XMFLOAT3(m_size.x * 2, m_size.y * 2, 0.0f));
            XMVECTOR t_translation = XMLoadFloat3(&XMFLOAT3(m_position.x, m_position.y, 1.0f));
            XMStoreFloat4x4(&m_transformMatrix, XMMatrixTransformation(t_origin, t_quater, t_scaling, t_origin, t_quater, t_translation));
            XMMATRIX t_matrix = XMMatrixTranspose(XMLoadFloat4x4(&m_transformMatrix));
            XMStoreFloat4x4(&m_transformMatrix, t_matrix);
        }

        Button::Button() {}

        Button::~Button() {}

        bool Button::CheckIfInside(int p_mousePosX, int p_mousePosY)
        {
            // Check if to the right of the button
            if(p_mousePosX > m_position.x + m_size.x)
            {
                m_materialInfo = m_buttonMaterials.m_vanillaMaterial;
                return false;
            }
            // Check if mouse is to the left of the button
            else if(p_mousePosX < m_position.x - m_size.x)
            {
                m_materialInfo = m_buttonMaterials.m_vanillaMaterial;
                return false;
            }
            // Check if mouse is over the button
            else if(p_mousePosY < m_position.y - m_size.y)
            {
                m_materialInfo = m_buttonMaterials.m_vanillaMaterial;
                return false;
            }
            // Check if mouse is under the button
            else if(p_mousePosY > m_position.y + m_size.y)
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
        }
    }
}