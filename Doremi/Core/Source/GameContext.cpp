#include <GameContext.hpp>
#include <EntityComponent/EntityHandler.hpp>
#include <EventHandler/EventHandler.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>

namespace Doremi
{
    namespace Core
    {
        GameContext* GameContext::m_singleton = nullptr;
        GameContext::GameContext(const DoremiEngine::Core::SharedContext& p_sharedContext)
            : m_entityHandler(EntityHandler()), m_eventHandler(EventHandler()), m_sharedContext(p_sharedContext)
        {
        }

        void GameContext::CreateGameContext(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            m_singleton = new GameContext(p_sharedContext);
        }


        GameContext& GameContext::GetInstance() { return *m_singleton; }
    }
}