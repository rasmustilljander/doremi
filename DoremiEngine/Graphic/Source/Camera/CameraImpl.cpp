#pragma once
#include <Internal/Camera/CameraImpl.hpp>
#include <string>

namespace DoremiEngine
{
    namespace Graphic
    {
        CameraImpl::CameraImpl() {}
        CameraImpl::~CameraImpl() {}
        void CameraImpl::SetProjectionMatrix(const DirectX::XMFLOAT4X4& p_matrix)
        {
            m_matrices.ProjectionMatrix = p_matrix;
            DirectX::XMMATRIX t_matrix = DirectX::XMLoadFloat4x4(&p_matrix);
            DirectX::XMMATRIX t_inverse = DirectX::XMMatrixInverse(&(DirectX::XMMatrixDeterminant(t_matrix)), t_matrix);
            DirectX::XMStoreFloat4x4(&(m_matrices.InverseProjection), t_inverse);
        }
        void CameraImpl::SetViewMatrix(const DirectX::XMFLOAT4X4& p_matrix) { m_matrices.ViewMatrix = p_matrix; }
        void CameraImpl::SetCameraPosition(const DirectX::XMFLOAT3& p_position) { m_matrices.CameraPosition = p_position; }
    }
}