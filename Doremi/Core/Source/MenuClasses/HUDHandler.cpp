#include <Doremi/Core/Include/MenuClasses/HUDHandler.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/HealthComponent.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>

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

        HUDHandler::HUDHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            DoremiEngine::Graphic::MeshManager& t_meshManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();
            DoremiEngine::Graphic::DirectXManager& t_dierctxManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager();

            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.18f, 0.04f);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.18f, 0.04f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);


            DoremiEngine::Graphic::SpriteInfo* t_spriteInfo = t_meshManager.BuildSpriteInfo(t_data);

            DoremiEngine::Graphic::MaterialInfo* t_matInfo = t_meshManager.BuildMaterialInfo("Healthbar.dds");
            ScreenObject t_barBar = ScreenObject(t_matInfo, t_spriteInfo);
            ScreenObject t_frontBackBar = ScreenObject(nullptr, nullptr);
            m_healthBar = Bar(t_barBar, t_frontBackBar, t_frontBackBar, 0.18f, 0.18f);

            m_bars.push_back(&m_healthBar);
        }

        HUDHandler::~HUDHandler() {}


        void HUDHandler::Update(double p_dt)
        {
            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());

            if(t_playerHandler->PlayerExists())
            {
                EntityID t_playerEntityID = t_playerHandler->GetPlayerEntityID();

                HealthComponent* t_healthComp = GetComponent<HealthComponent>(t_playerEntityID);

                float percentHealth = t_healthComp->currentHealth / t_healthComp->maxHealth;

                m_healthBar.UpdateProgress(percentHealth);
            }
        }
    }
}