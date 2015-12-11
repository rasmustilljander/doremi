#pragma once
#include <Manager/Manager.hpp>
#include <EventHandler/Subscriber.hpp>

#include <DirectXMath.h>
namespace DoremiEngine
{
    namespace Graphic
    {
        class Camera;
        class CameraManager; //TODOKO Change this name
    }
}
namespace Doremi
{
    namespace Core
    {
        /**
        Contains implementation for moving the camera, collision and what camera mode is active
        */
        enum class CameraType {FREELOOK, THIRDPERSON};
        class CameraManager :
            public Manager, public Subscriber
        {
        public:
            /**
            Dependent on the graphic module
            */
            CameraManager(const DoremiEngine::Core::SharedContext& p_sharedContext);

            virtual ~CameraManager();

            /**
            Updates the camera
            */
            void Update(double p_dt) override;

            /**
            TODOKO check is we need any event and write doc
            */
            void OnEvent(Event* p_event) override;
        private:
            CameraType m_currentCamera;
            DoremiEngine::Graphic::CameraManager& m_graphicModuleCameraManager;
            DoremiEngine::Graphic::Camera* m_thirdPersonCamera;
            DoremiEngine::Graphic::Camera* m_freeLookCamera;
        };
    }
}
