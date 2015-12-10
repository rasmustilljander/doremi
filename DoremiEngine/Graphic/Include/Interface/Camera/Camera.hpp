#pragma once
#include <string>
#include <DirectXMath.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        struct CameraMatrices
        {
            DirectX::XMFLOAT4X4 ProjectionMatrix;
            DirectX::XMFLOAT4X4 ViewMatrix;
            CameraMatrices() {}
        };
        class Camera
        {
            public:
            // TODOKO add documents
            virtual const CameraMatrices& GetCameraMatrices() const = 0;
            virtual void SetProjectionMatrix(const DirectX::XMFLOAT4X4& p_matrix) = 0;
            virtual void SetViewMatrix(const DirectX::XMFLOAT4X4& p_matrix) = 0;
        };
    }
}