#pragma once
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>

// Project specific

#include <DirectXMath.h>

namespace Doremi
{
    namespace Core
    {
        class EntityHandlerClient : public EntityHandler
        {
        public:
            static void StartupEntityHandlerClient();

            /**
            Checks events for add and remove objects
            */
            void OnEvent(Event* p_event) override;

        protected:
            EntityHandlerClient();
            ~EntityHandlerClient();
        };
    }
}