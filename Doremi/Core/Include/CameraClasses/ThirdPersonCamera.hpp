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
            ThirdPersonCamera(DoremiEngine::Graphic::Camera* p_camera);
            virtual ~ThirdPersonCamera();
            void Update(double p_dt);
            DoremiEngine::Graphic::Camera& GetCamera() const { return *m_camera; }
        private:
            DoremiEngine::Graphic::Camera* m_camera;
        };
    }
}