#pragma once
#include <string>
#include <DirectXMath.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        class Camera;
        class CameraManager
        {
            public:
            // TODOKO add documents
            virtual Camera* BuildNewCamera(DirectX::XMFLOAT4X4& p_projectionMatrix) = 0;
            virtual void PushCameraToDevice(const Camera& p_camera) = 0;
        };
    }
}