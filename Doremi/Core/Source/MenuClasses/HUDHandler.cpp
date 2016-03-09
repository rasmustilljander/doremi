#include <Doremi/Core/Include/MenuClasses/HUDHandler.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/HealthComponent.hpp>

// Events
#include <Doremi/Core/Include/EventHandler/EventHandler.hpp>
#include <Doremi/Core/Include/EventHandler/Events/DamageTakenEvent.hpp>

/// Engine
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Mesh/MaterialInfo.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>

#include <iostream>
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
            // Subscribe to events
            EventHandler::GetInstance()->Subscribe(EventType::DamageTaken, this);

            // Create HUD objects
            DoremiEngine::Graphic::MeshManager& t_meshManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();

            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.18f, 0.04f);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.18f, 0.04f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);


            DoremiEngine::Graphic::SpriteInfo* t_spriteInfo = t_meshManager.BuildSpriteInfo(t_data);
            DoremiEngine::Graphic::MaterialInfo* t_matInfo = t_meshManager.BuildMaterialInfo("HealthbarCropped_Health.dds");
            ScreenObject t_barBar = ScreenObject(t_matInfo, t_spriteInfo);

            t_spriteInfo = t_meshManager.BuildSpriteInfo(t_data);
            t_matInfo = t_meshManager.BuildMaterialInfo("HealthbarCropped_Frame.dds");
            ScreenObject t_frontBar = ScreenObject(t_matInfo, t_spriteInfo);

            t_spriteInfo = t_meshManager.BuildSpriteInfo(t_data);
            t_matInfo = t_meshManager.BuildMaterialInfo("HealthbarCropped_Background.dds");
            ScreenObject t_backBar = ScreenObject(t_matInfo, t_spriteInfo);

            m_healthBar = Bar(t_barBar, t_backBar, t_frontBar, 0.18f, 0.18f);

            t_data.halfsize = XMFLOAT2(0.5f, 0.5f);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.5f, 0.5f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);

            t_spriteInfo = t_meshManager.BuildSpriteInfo(t_data);
            t_matInfo = t_meshManager.BuildMaterialInfo("HitScreen.dds");
            m_bloodScreen.Object = ScreenObject(t_matInfo, t_spriteInfo);
            m_bloodScreen.MaxLifeTime = 1;

            m_bars.push_back(&m_healthBar);
        }

        HUDHandler::~HUDHandler() {}


        void HUDHandler::Update(double p_dt)
        {
            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());
            m_screenObjects.clear();
            if(t_playerHandler->PlayerExists())
            {
                EntityID t_playerEntityID = t_playerHandler->GetPlayerEntityID();

                HealthComponent* t_healthComp = GetComponent<HealthComponent>(t_playerEntityID);

                float percentHealth = t_healthComp->currentHealth / t_healthComp->maxHealth;

                m_healthBar.UpdateProgress(percentHealth);

                m_bloodScreen.LifeTime -= p_dt;

                if(m_bloodScreen.LifeTime > 0)
                {
                    m_bloodScreen.Object.m_materialInfo->SetAlpha(m_bloodScreen.LifeTime / m_bloodScreen.MaxLifeTime);
                    m_screenObjects.push_back(&m_bloodScreen.Object);
                }
            }
        }

        void HUDHandler::OnEvent(Event* p_event)
        {
            switch(p_event->eventType)
            {
                case EventType::DamageTaken:
                {
                    DamageTakenEvent* t_event = static_cast<DamageTakenEvent*>(p_event);
                    PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());

                    if(t_playerHandler->PlayerExists() && t_event->entityId == t_playerHandler->GetPlayerEntityID())
                    {
                        // The player took damage
                        std::cout << "setting lifetime on blood screen" << std::endl;
                        m_bloodScreen.LifeTime = m_bloodScreen.MaxLifeTime;
                    }


                    break;
                }
            }
        }
    }
}