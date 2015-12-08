#pragma once
#include <Manager/Manager.hpp>
#include <EventHandler/Subscriber.hpp>
namespace Doremi
{
    namespace Core
    {
        /**
        Contains implementation for moving the camera, collision and what camera mode is active
        */
        class CameraManager :
            public Manager, public Subscriber
        {
        public:
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

        };
    }
}
