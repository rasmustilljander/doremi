#pragma once
#include <string>
#include <DirectXMath.h>

namespace DoremiEngine
{
    namespace Graphic
    {
        class Camera;
        /**
        Builds new cameras and pushes cameras to the GPU
        */
        class CameraManager
        {
        public:
            /**
            Creates a new Camera from the given projection matrix.
            */
            virtual Camera* BuildNewCamera(DirectX::XMFLOAT4X4& p_projectionMatrix) = 0;
            /**
            Sends the matrices in the given camera class to the GPU
            */
            virtual void PushCameraToDevice(const Camera& p_camera) = 0;
        };
    }
}