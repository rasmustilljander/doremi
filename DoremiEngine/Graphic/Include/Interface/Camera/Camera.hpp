#pragma once
#include <string>
#include <DirectXMath.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct CameraMatrices
        {
            DirectX::XMFLOAT4X4 ViewMatrix;
            DirectX::XMFLOAT4X4 ProjectionMatrix;
            DirectX::XMFLOAT4X4 InverseProjection;
            DirectX::XMFLOAT3 CameraPosition;
            float pad;
            CameraMatrices() {}
        };
        /**
        Contains information on view and projection matrix. The user needs one of these classes to change the view and projection matrices in the
        shaders
        */
        class Camera
        {
        public:
            virtual const CameraMatrices& GetCameraMatrices() const = 0;
            virtual const DirectX::XMFLOAT3& GetCameraPosition() const = 0;
            virtual void SetProjectionMatrix(const DirectX::XMFLOAT4X4& p_matrix) = 0;
            virtual void SetViewMatrix(const DirectX::XMFLOAT4X4& p_matrix) = 0;
            virtual void SetCameraPosition(const DirectX::XMFLOAT3& p_position) = 0;
        };
    }
}