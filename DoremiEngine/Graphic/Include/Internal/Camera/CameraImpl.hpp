#pragma once
#include <Interface/Camera/Camera.hpp>
#include <string>
namespace DirectX
{
    struct XMFLOAT4X4;
}
namespace DoremiEngine
{
    namespace Graphic
    {
        class CameraImpl : public Camera
        {
            public:
            // TODOKO add documents
            CameraImpl();
            virtual ~CameraImpl();
            const CameraMatrices& GetCameraMatrices() const { return m_matrices; };
            void SetProjectionMatrix(const DirectX::XMFLOAT4X4& p_matrix);
            void SetViewMatrix(const DirectX::XMFLOAT4X4& p_matrix);

            private:
            CameraMatrices m_matrices;
        };
    }
}