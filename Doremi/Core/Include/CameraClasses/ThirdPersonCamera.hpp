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
            void Update();
            void UpdateInput(const double& p_dt);
            DoremiEngine::Graphic::Camera& GetCamera() const { return *m_camera; }
        private:
            // Help functions
            DoremiEngine::Graphic::Camera* m_camera;
            float m_angle;
            float m_distanceFromPlayer;
            float m_maxAngle;
            float m_minAngle;
        };
    }
}