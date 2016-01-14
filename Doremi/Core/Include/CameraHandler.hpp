#pragma once

#include <DirectXMath.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        class Camera;
        class CameraManager; // TODOKO Change this name
    }
    namespace Core
    {
        class SharedContext;
    }
}
namespace Doremi
{
    namespace Core
    {
        /**
        Contains implementation for moving the camera, collision and what camera mode is active
        */
        enum class CameraType
        {
            FREELOOK,
            THIRDPERSON
        };
        class ThirdPersonCamera;
        class FreeLookCamera;
        class CameraHandler
        {
            public:
            static CameraHandler* GetInstance();
            static void StartCameraHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);
            virtual ~CameraHandler();

            /**
            Updates the camera
            */
            void Update(double p_dt);

            void ChangeCamera(CameraType p_type);

            DirectX::XMFLOAT3 GetActiveCameraPosition();

            private:
            /**
            Dependent on the graphic module
            */
            CameraHandler(const DoremiEngine::Core::SharedContext& p_sharedContext);

            static CameraHandler* m_singleton;
            const DoremiEngine::Core::SharedContext& m_sharedContext;
            CameraType m_currentCamera;
            ThirdPersonCamera* m_thirdPersonCamera;
            FreeLookCamera* m_freeLookCamera;
        };
    }
}
