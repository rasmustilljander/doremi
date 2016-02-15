// Project specific
#include <Doremi/Core/Include/MenuClasses/ScreenObject.hpp>
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
        ScreenObject::ScreenObject(DoremiEngine::Graphic::MaterialInfo* p_materialInfo, DoremiEngine::Graphic::MeshInfo* p_meshName,
                                   const XMFLOAT2& p_position, const XMFLOAT2& p_size)
            : m_materialInfo(p_materialInfo), m_meshInfo(p_meshName), m_position(p_position), m_extent(p_size)
        {
            // Building a transform matrix. needs no rotation or scaling orientation the variable can be reused. Scaling origin is 0,0,0 for the quad
            XMVECTOR t_origin = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f));
            XMVECTOR t_quater = XMLoadFloat4(&XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
            XMVECTOR t_scaling = XMLoadFloat3(&XMFLOAT3(m_extent.x * 2, m_extent.y * 2, 0.0f));
            XMVECTOR t_translation = XMLoadFloat3(&XMFLOAT3(m_position.x, m_position.y, 1.0f));
            XMStoreFloat4x4(&m_transformMatrix, XMMatrixTransformation(t_origin, t_quater, t_scaling, t_origin, t_quater, t_translation));
            XMMATRIX t_matrix = XMMatrixTranspose(XMLoadFloat4x4(&m_transformMatrix));
            XMStoreFloat4x4(&m_transformMatrix, t_matrix);
        }
        ScreenObject::ScreenObject() {}
        ScreenObject::~ScreenObject() {}
    }
}