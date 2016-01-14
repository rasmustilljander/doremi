#pragma once
#include <DirectXMath.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        class Camera;
    }
}
namespace Doremi
{
    namespace Core
    {
        class FreeLookCamera
        {
        public:
            FreeLookCamera(DoremiEngine::Graphic::Camera* p_camera);
            virtual ~FreeLookCamera();
            void Update(double p_dt);
            DoremiEngine::Graphic::Camera& GetCamera() const { return *m_camera; }
        private:
            void HandleUserCMD(double pDT);

            DoremiEngine::Graphic::Camera* m_camera;

            DirectX::XMFLOAT3 m_camUp;
            DirectX::XMFLOAT3 m_camTarget;
            DirectX::XMFLOAT3 m_camPos;
            DirectX::XMFLOAT3 m_defaultForward;
            DirectX::XMFLOAT3 m_defaultRight;
            DirectX::XMFLOAT3 m_camForward;
            DirectX::XMFLOAT3 m_camRight;
            DirectX::XMFLOAT4X4 m_camRotationMatrix;

            float m_camYaw;
            float m_camPitch;
            float m_moveLeftRight;
            float m_moveForwardBackward;
            float m_speed;
        };
    }
}