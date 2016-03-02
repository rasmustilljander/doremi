#include <Doremi/Core/Include/MenuClasses/HUDHandler.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/HealthComponent.hpp>

namespace Doremi
{
    namespace Core
    {
        HUDHandler* HUDHandler::m_singleton = nullptr;

        HUDHandler * HUDHandler::GetInstance()
        {
            if (m_singleton == nullptr)
            {
                std::runtime_error("GetInstance called before StartHUDHandler");
            }
            return m_singleton;
        }

        void HUDHandler::StartHUDHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("StartHUDHandler called multiple times");
            }
            m_singleton = new HUDHandler(p_sharedContext);
        }

        HUDHandler::HUDHandler(const DoremiEngine::Core::SharedContext & p_sharedContext) : m_sharedContext(p_sharedContext)
        {

        }

        HUDHandler::~HUDHandler()
        {
        }


        void HUDHandler::Update(double p_dt)
        {
            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());

            if(t_playerHandler->PlayerExists())
            {
                EntityID t_playerEntityID = t_playerHandler->GetPlayerEntityID();

                HealthComponent* t_healthComp = GetComponent<HealthComponent>(t_playerEntityID);

                float percentHealth = t_healthComp->currentHealth / t_healthComp->maxHealth;
            }

            return;
        }
    }
}