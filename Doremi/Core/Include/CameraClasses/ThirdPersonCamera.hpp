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
        class ThirdPersonCamera
        {
        public:
            /**
            angles are in radians
            */
            ThirdPersonCamera(DoremiEngine::Graphic::Camera* p_camera, const float& p_distanceFromPlayer);
            virtual ~ThirdPersonCamera();
            void UpdatePositions();
            void UpdateInterpolation(const double& p_alpha);
            DoremiEngine::Graphic::Camera& GetCamera() const { return *m_camera; }
        private:
            DoremiEngine::Graphic::Camera* m_camera;
            float m_angle;
            float m_distanceFromPlayer;
            float m_maxAngle;
            float m_minAngle;

            // Used to interpolate to wanted view matrix
            float m_interpolationSpeed;

            // Saves the previous position and focus for interpolation
            DirectX::XMFLOAT3 m_previousPosition;
            DirectX::XMFLOAT3 m_previousFocus;

            // Saves the next position and focus for interpolation
            DirectX::XMFLOAT3 m_nextPosition;
            DirectX::XMFLOAT3 m_nextFocus;
        };
    }
}