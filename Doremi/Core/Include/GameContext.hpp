#pragma once

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
        // Forward Declarations
        class EntityHandler;
        class EventHandler;


        class GameContext
        {
            public:
            static GameContext& GetInstance();

            // this works as our constructor
            static void CreateGameContext(const DoremiEngine::Core::SharedContext& p_sharedContext);
            static void ReleaseGameContext();

            EntityHandler& m_entityHandler;
            EventHandler& m_eventHandler;
            const DoremiEngine::Core::SharedContext& m_sharedContext;

            private:
            static GameContext* m_singleton;
            GameContext(const DoremiEngine::Core::SharedContext& p_sharedContext);
            ~GameContext();
        };
    }
}