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
            Removes the entire entity at the specific location
            */
            void RemoveEntity(int p_entityID);

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