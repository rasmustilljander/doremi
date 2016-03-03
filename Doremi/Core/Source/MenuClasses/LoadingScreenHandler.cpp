#include <Doremi/Core/Include/MenuClasses/LoadingScreenHandler.hpp>
#include <DoremiEngine/Core/Include/SharedContext.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <Doremi/Core/Include/EntityComponent/EntityHandler.hpp>
#include <Doremi/Core/Include/EntityComponent/Components/HealthComponent.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/MeshManager.hpp>
#include <DoremiEngine/Graphic/Include/Interface/Manager/DirectXManager.hpp>
#include <DoremiEngine/Graphic/Include/GraphicModule.hpp>
#include <Doremi/Core/Include/PlayerHandlerClient.hpp>
#include <iostream>

namespace Doremi
{
    namespace Core
    {
        LoadingScreenHandler* LoadingScreenHandler::m_singleton = nullptr;

        LoadingScreenHandler* LoadingScreenHandler::GetInstance()
        {
            if(m_singleton == nullptr)
            {
                std::runtime_error("GetInstance called before StartLoadingScreenHandler");
            }
            return m_singleton;
        }

        void LoadingScreenHandler::StartLoadingScreenHandler(const DoremiEngine::Core::SharedContext& p_sharedContext)
        {
            if(m_singleton != nullptr)
            {
                std::runtime_error("StartLoadingScreenHandler called multiple times");
            }
            m_singleton = new LoadingScreenHandler(p_sharedContext);
        }

        LoadingScreenHandler::LoadingScreenHandler(const DoremiEngine::Core::SharedContext& p_sharedContext) : m_sharedContext(p_sharedContext)
        {
            DoremiEngine::Graphic::MeshManager& t_meshManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetMeshManager();
            DoremiEngine::Graphic::DirectXManager& t_dierctxManager = p_sharedContext.GetGraphicModule().GetSubModuleManager().GetDirectXManager();

            // Create background
            DoremiEngine::Graphic::SpriteData t_data;

            t_data.halfsize = XMFLOAT2(0.5f, 0.5f);
            t_data.origo = XMFLOAT2(0.0f, 0.0f);
            t_data.position = XMFLOAT2(0.5f, 0.5f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);

            DoremiEngine::Graphic::SpriteInfo* t_spriteInfo = t_meshManager.BuildSpriteInfo(t_data);
            DoremiEngine::Graphic::MaterialInfo* t_matInfo = t_meshManager.BuildMaterialInfo("LoadingScreen_v1.dds");

            m_background = ScreenObject(t_matInfo, t_spriteInfo);

            m_screenObjects.push_back(&m_background);

            // Create loading bar
            t_data.halfsize = XMFLOAT2(0.4f, 0.04f);
            t_data.origo = XMFLOAT2(0.4f, 0.0f);
            t_data.position = XMFLOAT2(0.1f, 0.9f);
            t_data.txtPos = XMFLOAT2(0.0f, 0.0f);
            t_data.txtSize = XMFLOAT2(1.0f, 1.0f);


            DoremiEngine::Graphic::SpriteInfo* t_spriteInfoFront = t_meshManager.BuildSpriteInfo(t_data);
            DoremiEngine::Graphic::SpriteInfo* t_spriteInfoBack = t_meshManager.BuildSpriteInfo(t_data);
            DoremiEngine::Graphic::SpriteInfo* t_spriteInfoBar = t_meshManager.BuildSpriteInfo(t_data);

            DoremiEngine::Graphic::MaterialInfo* t_matInfoFront = t_meshManager.BuildMaterialInfo("LoadingBarTop.dds");
            DoremiEngine::Graphic::MaterialInfo* t_matInfoBack = t_meshManager.BuildMaterialInfo("LoadingBarBack.dds");
            DoremiEngine::Graphic::MaterialInfo* t_matInfoBar = t_meshManager.BuildMaterialInfo("LoadingBarBar.dds");

            ScreenObject t_backBar = ScreenObject(t_matInfoBack, t_spriteInfoBack);
            ScreenObject t_frontBar = ScreenObject(t_matInfoFront, t_spriteInfoFront);
            ScreenObject t_barBar = ScreenObject(t_matInfoBar, t_spriteInfoBar);
            m_loadingBar = Bar(t_barBar, t_backBar, t_frontBar, 0.1f, 0.4f);

            m_bars.push_back(&m_loadingBar);

            m_loadingBar.UpdateProgress(0.0f);
        }

        LoadingScreenHandler::~LoadingScreenHandler() {}

        void LoadingScreenHandler::Reset() { m_loadingBar.UpdateProgress(0.0f); }

        void LoadingScreenHandler::Update(double p_dt)
        {
            PlayerHandlerClient* t_playerHandler = static_cast<PlayerHandlerClient*>(PlayerHandler::GetInstance());


            uint32_t t_maxEvent = t_playerHandler->GetMaximumNumberOfJoinEvents();
            uint32_t t_curEvent = t_playerHandler->GetLastJoinEventRead();

            // Couldn't be arsed to do a std::max function, setting maxevent = 1 mean percentDone = 0
            if(t_maxEvent == 0)
            {
                t_maxEvent = 1;
            }

            // Loading world is 50% =D
            float percentDone = 0.5f + 0.5f * static_cast<float>(t_curEvent) / static_cast<float>(t_maxEvent);

            m_loadingBar.UpdateProgress(percentDone);
        }
    }
}