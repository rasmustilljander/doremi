#pragma once
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>

// Project specific

#include <DirectXMath.h>

namespace DoremiEngine
{
    namespace Core
    {
        class SharedContext;
    }
}

namespace Doremi
{
    namespace Core
    {
        class EntityHandlerClient : public EntityHandler
        {
        public:
            static void StartupEntityHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext);

            /**
            Removes the entire entity at the specific location
            */
            void RemoveEntity(int p_entityID);


            /**
            Checks events for add and remove objects
            */
            void OnEvent(Event* p_event) override;

        protected:
            explicit EntityHandlerClient(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~EntityHandlerClient();

            void ResetWorld();

            const DoremiEngine::Core::SharedContext& m_sharedContext;
        };
    }
}