#include <Camera/CameraSystem.hpp>
#include <DirectXMath.h>


namespace DoremiEngine
{
    namespace Graphic
    {
        CameraSystem::CameraSystem()
        {
            m_screenHeight = 800; // TODO get these from a config file?
            m_screenWidth = 800;
            m_angle = 60.0f;
            m_viewDistance = 1000;
            UpdateProjectionMatrix();
        }

        CameraSystem::~CameraSystem() {}

        void CameraSystem::SetViewMatrix(const DirectX::XMFLOAT3& p_position, const DirectX::XMFLOAT3& p_direction, const DirectX::XMFLOAT3& p_up)
        {
            using namespace DirectX;
            XMVECTOR position = XMLoadFloat3(&p_position);
            XMVECTOR direction = XMLoadFloat3(&p_direction);
            XMVECTOR up = XMLoadFloat3(&p_up);

            XMMATRIX temp = DirectX::XMMatrixLookAtLH(position, direction, up);
            XMStoreFloat4x4(m_viewMatrix, temp);
        }

        void CameraSystem::SetFieldOfView(float p_angle)
        {
            m_angle = p_angle;
            UpdateProjectionMatrix();
        }

        void CameraSystem::UpdateProjectionMatrix()
        {
            DirectX::XMMATRIX temp = DirectX::XMMatrixPerspectiveFovLH(m_angle * DirectX::XM_PI / 180.0f, m_screenHeight / m_screenWidth, 0.2, m_viewDistance);
            DirectX::XMStoreFloat4x4(m_projectionMatrix, temp);
        }
    }
}
